# M5Stack Core Dashboard

Wetter-, Kalender- und MVG-Abfahrten-Dashboard für den M5Stack Core (ESP32),
gesteuert über die 3 eingebauten Buttons (A, B, C).

Details zu Funktionsumfang, API-Formaten und Design-Entscheidungen stehen in
`SPEC-final.md` im Space. Umsetzungsschritte stehen in `PLAN.md` und `TODO.md`.

## Setup

1. PlatformIO CLI installieren:
   ```
   pip install -U platformio
   ```

2. WLAN-Zugangsdaten eintragen:
   ```
   cp include/secrets.h.example include/secrets.h
   ```
   Dann in `include/secrets.h` `WIFI_SSID` und `WIFI_PASSWORD` anpassen.
   Die drei API-URLs sind bereits vorbefüllt.

3. Gerät per USB anschließen.

## Build & Flash

```
./scripts/deploy.sh
```

Falls mehrere serielle Geräte angeschlossen sind und die automatische
Port-Erkennung fehlschlägt, kann der Port explizit übergeben werden:

```
./scripts/deploy.sh /dev/ttyUSB0
```

Das Skript baut nur und flasht, es öffnet keinen seriellen Monitor.

Falls du den Monitor separat sehen willst:

```
pio device monitor
```

## Bedienung

| Button | Funktion |
|---|---|
| A | Wechselt zwischen Wetter-Detail und Kalender-Detail |
| B | Zurück zur Hauptseite |
| C | MVG-Abfahrtsseite |

Nach 5 Minuten ohne Tastendruck springt das Gerät automatisch zurück zur
Hauptseite. Die Hauptseite aktualisiert sich alle 10 Minuten, die
MVG-Seite jede Minute.

## Icons

Die Icons (Sonne, Wolke, Regen, U-/S-Bahn-Badges, Kalender, Fehler-Symbol)
werden aktuell prozedural mit M5Stack-Grafikprimitiven gezeichnet
(`src/icons/icons.h`), um den Flash-Speicher zu schonen. Für echte
Pixel-Art-Bitmaps können die Funktionskörper später durch
`M5.Lcd.drawBitmap(...)`-Aufrufe mit RGB565-Arrays ersetzt werden.

## Git

Dieses Projekt ist bewusst noch nicht als Git-Repository initialisiert.
Sobald gewünscht:

```
git init
git add .
git commit -m "Initial M5Stack dashboard"
```

`include/secrets.h` ist bereits in `.gitignore` ausgeschlossen.
