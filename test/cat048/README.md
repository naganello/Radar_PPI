# Simulatore ASTERIX CAT048

Genera e invia tracce ASTERIX CAT048 su multicast UDP per testare la ricezione di plot/track.

## Utilizzo

```powershell
python test\cat048\send_cat048.py
```

## Dettagli

Il simulatore crea due tracce aeree:
- **Track 1001**: parte da 3 NM a Nord, direzione Sud
- **Track 1002**: parte da 4 NM a Ovest, direzione Est

Ogni traccia viene codificata in un record CAT048 con:
- Data Source Identifier (FRN 1)
- Target Report Descriptor (FRN 3)
- Measured Position in Slant Polar Coordinates (FRN 4)
- Track Number (FRN 11)

I pacchetti vengono inviati periodicamente al gruppo multicast configurato.

## Opzioni

| Opzione | Default | Descrizione |
|---|---|---|
| `--group` | 239.1.1.48 | Gruppo multicast |
| `--port` | 30048 | Porta UDP |
| `--period` | 1.0 | Intervallo tra invii (secondi) |
| `--ttl` | 2 | TTL multicast |
| `--track1-speed` | 6.0 | Velocità traccia 1 (NM/min) |
| `--track2-speed` | 9.0 | Velocità traccia 2 (NM/min) |

## Esempi

Inviare ogni 2 secondi con TTL 5:
```powershell
python test\cat048\send_cat048.py --period 2.0 --ttl 5
```

Usare gruppo e porta personalizzati:
```powershell
python test\cat048\send_cat048.py --group 239.1.1.99 --port 40048
```
