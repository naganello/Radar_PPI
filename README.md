# Radar_ppi 0.4.5

Release completa del progetto `Radar_ppi` con rendering OpenGL e PPI professionale.

## Funzionalità incluse

- Ricezione multicast CAT048 e CAT240.
- Decodifica ASTERIX CAT048 e CAT240.
- PPI basato su `QOpenGLWidget`.
- Rendering CAT240 tramite texture persistente.
- Doppio buffer e triple buffering.
- Decay video/sweep.
- Zoom interattivo con rotellina mouse.
- Scala dinamica 1..25 NM.
- Calcolo posizione mouse nel PPI:
  - metri;
  - NM;
  - azimuth;
  - X/Y metri.
- Selezione plot/traccia con click mouse.
- Overlay caratteristiche della selezione.
- Salvataggio/caricamento socket multicast in XML.
- File `.clang-format` incluso.

## XML socket config

Formato:

```xml
<RadarSocketConfig>
    <CAT048 group="239.1.1.48" port="30048" local="0.0.0.0"/>
    <CAT240 group="239.1.1.240" port="30240" local="0.0.0.0"/>
</RadarSocketConfig>
```

Menu:

```text
Multicast -> Carica socket XML...
Multicast -> Salva socket XML...
```

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Esecuzione

```bash
./build/bin/Radar_ppi
```
