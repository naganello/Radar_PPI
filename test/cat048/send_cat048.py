#!/usr/bin/env python3

import argparse

import math

import socket

import struct

import time

 

CAT048 = 48

 

DEFAULT_GROUP = "239.1.1.48"

DEFAULT_PORT = 30048

 

SAC = 1

SIC = 1

 

def clamp_u16(value: int) -> int:

    return max(0, min(0xFFFF, value))

 

def nm_xy_to_polar(x_nm: float, y_nm: float) -> tuple[float, float]:

    """

    Coordinate:

      x_nm > 0 = Est

      y_nm > 0 = Nord

 

    Azimuth ASTERIX:

      0 deg   = Nord

      90 deg  = Est

      180 deg = Sud

      270 deg = Ovest

    """

    range_nm = math.hypot(x_nm, y_nm)

 

    if range_nm <= 0.0:

        return 0.0, 0.0

 

    azimuth_deg = math.degrees(math.atan2(x_nm, y_nm))

    if azimuth_deg < 0.0:

        azimuth_deg += 360.0

 

    return range_nm, azimuth_deg

 

def encode_cat048_record(

    sac: int,

    sic: int,

    track_number: int,

    range_nm: float,

    azimuth_deg: float,

) -> bytes:

    """

    Record CAT048 minimale con:

      FRN 1  -> I048/010 Data Source Identifier

      FRN 3  -> I048/020 Target Report Descriptor

      FRN 4  -> I048/040 Measured Position in Slant Polar Coordinates

      FRN 11 -> I048/161 Track Number

 

    FSPEC:

      Octet 1:

        bit 8 FRN1 = 1

        bit 6 FRN3 = 1

        bit 5 FRN4 = 1

        bit 1 FX   = 1

        => 0xB1

 

      Octet 2:

        bit 5 FRN11 = 1

        FX = 0

        => 0x10

    """

 

    fspec_1 = 0xB1

    fspec_2 = 0x10

 

    # I048/010

    data_source = struct.pack(">BB", sac & 0xFF, sic & 0xFF)

 

    # I048/020 Target Report Descriptor

    # Valore semplice senza estensione.

    # Typ impostato a detection/plot generico.

    target_report_descriptor = struct.pack(">B", 0x20)

 

    # I048/040

    # RHO LSB = 1/256 NM

    # THETA LSB = 360 / 65536 deg

    rho_raw = clamp_u16(round(range_nm * 256.0))

    theta_raw = clamp_u16(round((azimuth_deg % 360.0) * 65536.0 / 360.0))

 

    measured_position = struct.pack(">HH", rho_raw, theta_raw)

 

    # I048/161

    track_raw = track_number & 0x0FFF

    track_number_item = struct.pack(">H", track_raw)

 

    return b"".join(

        [

            struct.pack(">BB", fspec_1, fspec_2),

            data_source,

            target_report_descriptor,

            measured_position,

            track_number_item,

        ]

    )

 

def encode_cat048_block(records: list[bytes]) -> bytes:

    payload = b"".join(records)

    length = 3 + len(payload)

 

    return struct.pack(">BH", CAT048, length) + payload

 

class SimulatedTrack:

    def __init__(

        self,

        track_number: int,

        x_nm: float,

        y_nm: float,

        vx_nm_per_min: float,

        vy_nm_per_min: float,

    ):

        self.track_number = track_number

        self.x_nm = x_nm

        self.y_nm = y_nm

        self.vx_nm_per_min = vx_nm_per_min

        self.vy_nm_per_min = vy_nm_per_min

 

    def update(self, dt_seconds: float) -> None:

        dt_minutes = dt_seconds / 60.0

        self.x_nm += self.vx_nm_per_min * dt_minutes

        self.y_nm += self.vy_nm_per_min * dt_minutes

 

    def to_cat048_record(self) -> bytes:

        range_nm, azimuth_deg = nm_xy_to_polar(self.x_nm, self.y_nm)

 

        return encode_cat048_record(

            sac=SAC,

            sic=SIC,

            track_number=self.track_number,

            range_nm=range_nm,

            azimuth_deg=azimuth_deg,

        )

 

def create_socket(ttl: int) -> socket.socket:

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)

    sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, struct.pack("B", ttl))

    return sock

 

def main() -> None:

    parser = argparse.ArgumentParser(

        description="Simulatore UDP multicast ASTERIX CAT048 con due tracce."

    )

 

    parser.add_argument("--group", default=DEFAULT_GROUP, help="Gruppo multicast")

    parser.add_argument("--port", type=int, default=DEFAULT_PORT, help="Porta UDP")

    parser.add_argument("--period", type=float, default=1.0, help="Periodo invio in secondi")

    parser.add_argument("--ttl", type=int, default=1, help="TTL multicast")

 

    parser.add_argument(

        "--track1-speed",

        type=float,

        default=2.0,

        help="Velocità track 1 verso Sud in NM/min",

    )

 

    parser.add_argument(

        "--track2-speed",

        type=float,

        default=3.0,

        help="Velocità track 2 verso Est in NM/min",

    )

 

    args = parser.parse_args()

 

    # Track 1:

    # parte da 3 NM Nord, va verso Sud.

    track1 = SimulatedTrack(

        track_number=1001,

        x_nm=0.0,

        y_nm=3.0,

        vx_nm_per_min=0.0,

        vy_nm_per_min=-abs(args.track1_speed),

    )

 

    # Track 2:

    # parte da 4 NM Ovest, va verso Est.

    track2 = SimulatedTrack(

        track_number=1002,

        x_nm=-4.0,

        y_nm=0.0,

        vx_nm_per_min=abs(args.track2_speed),

        vy_nm_per_min=0.0,

    )

 

    tracks = [track1, track2]

 

    sock = create_socket(args.ttl)

    destination = (args.group, args.port)

 

    print(f"Invio CAT048 multicast verso {args.group}:{args.port}")

    print("Track 1001: start 3 NM Nord -> Sud")

    print("Track 1002: start 4 NM Ovest -> Est")

    print("CTRL+C per terminare")

 

    last_time = time.monotonic()

 

    try:

        while True:

            now = time.monotonic()

            dt = now - last_time

            last_time = now

 

            for track in tracks:

                track.update(dt)

 

            records = [track.to_cat048_record() for track in tracks]

            packet = encode_cat048_block(records)

 

            sock.sendto(packet, destination)

 

            for track in tracks:

                range_nm, azimuth_deg = nm_xy_to_polar(track.x_nm, track.y_nm)

                print(

                    f"Track {track.track_number} | "

                    f"x={track.x_nm:+.3f} NM "

                    f"y={track.y_nm:+.3f} NM | "

                    f"R={range_nm:.3f} NM "

                    f"Az={azimuth_deg:.2f} deg"

                )

 

            print(f"Pacchetto CAT048 inviato: {len(packet)} bytes")

            print("-" * 72)

 

            time.sleep(args.period)

 

    except KeyboardInterrupt:

        print("Simulazione terminata.")

 

    finally:

        sock.close()

 

if __name__ == "__main__":

    main()