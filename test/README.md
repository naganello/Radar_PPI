# Test — Simulatori ASTERIX

Questa directory contiene simulatori Python per generare traffico ASTERIX su multicast UDP, utili per testare l'applicazione Radar_ppi.

## Struttura

```
test/
├── cat048/           # Simulatore CAT048 (track data)
│   └── send_cat048.py
└── cat240 4 sec/     # Simulatore CAT240 (video radar)
    ├── AsterixCAT240.py
    ├── main.py
    └── readme.md
```

## Prerequisiti

- Python 3.12+
- Nessuna dipendenza esterna (solo libreria standard)

## Avvio rapido

### CAT048 — Invio tracce simulate

```powershell
python test\cat048\send_cat048.py
```

Invia due tracce (Track 1001 e 1002) al gruppo multicast `239.1.1.48:30048` con periodo 1 secondo.

Opzioni:
- `--group GROUP`       gruppo multicast (default: 239.1.1.48)
- `--port PORT`         porta UDP (default: 30048)
- `--period SEC`        periodo di invio in secondi (default: 1.0)
- `--ttl TTL`           TTL multicast (default: 2)
- `--track1-speed NM/MIN` velocità traccia 1 in NM/min (default: 6.0)
- `--track2-speed NM/MIN` velocità traccia 2 in NM/min (default: 9.0)

### CAT240 — Video radar simulato

```powershell
python test\cat240 4 sec\main.py
```

Invia video radar rotante (512 radiali, rotazione ogni 4 sec) al gruppo multicast `239.0.0.1:4433`.

Per personalizzare il pattern video, modificare il parametro `PATTERN` in `main.py` (1, 2 o 3).
