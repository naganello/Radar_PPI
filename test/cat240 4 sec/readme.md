# Simulatore ASTERIX CAT240

Trasmissione di video radar rotante su multicast UDP.

## Utilizzo

```powershell
python "test\cat240 4 sec\main.py" [--pattern {1,2,3}]
```

## Opzioni

| Opzione | Default | Descrizione |
|---|---|---|
| `--pattern` | 2 | Pattern video: 1 (gradiente), 2 (costante 0x66), 3 (costante 0x33) |

## Esempi

Pattern 1 (gradiente):
```powershell
python "test\cat240 4 sec\main.py" --pattern 1
```

Pattern 3 (costante 0x33):
```powershell
python "test\cat240 4 sec\main.py" --pattern 3
```

## Note

- Invia 512 radiali per rotazione completa (360°)
- Periodo di rotazione: 4 secondi
- Gruppo multicast: `239.0.0.1:4433`
- Invia anche pacchetto DSNOR discovery su `227.228.229.230:59368`

## Riferimenti

- Cambridge Pixel RadarView: https://cambridgepixel.com/products/display-applications/radarview/
