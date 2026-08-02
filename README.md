# WT32-SC01 Plus Dashboard

Wetter-, Kalender- und MVG-Abfahrten-Dashboard für den WT32-SC01 Plus
(ESP32-S3, 3.5" kapazitiver Touch-IPS-Display, 320x480 im Hochformat),
gesteuert ausschließlich über Touch. Vollständig übernommene Backend-APIs
und Fachlogik aus dem [M5Stack-Vorgängerprojekt](https://github.com/skoelle/m5stack-dashboard),
Display- und Eingabeschicht neu aufgebaut mit LovyanGFX + LVGL 9.

Details zu Funktionsumfang, API-Formaten und Design-Entscheidungen
stehen in [`SPEC.md`](SPEC.md). Die Migrationshistorie (Vorgänger-Spec,
Migrationsplan, Task-Liste) liegt unter [`docs/`](docs/).

## Setup

1. PlatformIO CLI installieren:
   ```
   pip install -U platformio
   ```

2. WLAN-Zugangsdaten + API-URLs eintragen:
   ```
   cp include/secrets.h.example include/secrets.h
   ```
   Dann in `include/secrets.h` `WIFI_SSID`, `WIFI_PASSWORD` und die drei
   `*_API_URL`-Konstanten anpassen. `include/secrets.h` ist über
   `.gitignore` vom Repo ausgeschlossen.

3. (Optional) Pinbelegung verifizieren: `include/board_pins.h` enthält die
   Referenz-Pinout für das WT32-SC01 Plus (ST7796 8-Bit-Parallel +
   FT6336U I2C-Touch). Bei abweichender Board-Revision hier anpassen.

4. Gerät per USB anschließen.

## Build & Flash

**Linux/macOS:**
```
./scripts/deploy.sh
```

**Windows:**
```
scripts\deploy.cmd
```

Falls mehrere serielle Geräte angeschlossen sind und die automatische
Port-Erkennung fehlschlägt, kann der Port explizit übergeben werden:

```
./scripts/deploy.sh /dev/ttyUSB0
scripts\deploy.cmd COM3
```

Das Skript baut nur und flasht, es öffnet keinen seriellen Monitor.

Nur Build (ohne Flash):
```
./scripts/build.sh
scripts\build.cmd
```

Falls du den Monitor separat sehen willst:

```
pio device monitor
```

## Bedienung

| Touch | Funktion |
|---|---|
| Tap auf Wetter-Kachel (Home) | Wetter-Detailseite |
| Tap auf Kalender-Kachel (Home) | Kalender-Detailseite |
| Tap auf MVG-Kachel (Home) | MVG-Abfahrtsseite |
| Tap auf Zurück-Button (unten links, Detailseiten) | Zurück zur Hauptseite |
| Tap auf Wetter-Kachel im Fehlerzustand | Manueller Retry |

Nach 5 Minuten ohne Touch-Eingabe springt das Gerät automatisch zurück
zur Hauptseite. Die Hauptseite aktualisiert sich alle 10 Minuten, die
MVG-Seite jede Minute.

## Fehlerbehandlung

Wenn eine API nicht erreichbar ist, wird auf der betroffenen Kachel bzw. dem Screen eine Fehleranzeige gezeigt (Text + ggf. Retry-Icon). Es wird kein "letzter bekannter Wert" angezeigt.

**Retry:**
- Automatisch beim nächsten regulären Refresh-Intervall (10 Min. bzw. 1 Min.)
- Manuell durch erneutes Antippen der Kachel im Fehlerzustand

## Projektstruktur

```
/
├── platformio.ini              esp32-s3, LovyanGFX + LVGL9
├── lv_conf.h                   LVGL-Konfiguration
├── include/
│   ├── board_pins.h            WT32-SC01-Plus-Pinout
│   ├── secrets.h.example       Platzhalter für WLAN/API-URLs
│   ├── theme.h                 Dark-Mode-Farbschema (LVGL)
│   ├── text_utils.h            UTF-8->ASCII-Transliteration
│   └── date_utils.h            Datumsformatierung (kein NTP)
├── src/
│   ├── main.cpp                Screen-Controller + Inaktivitäts-Timer
│   ├── display/                LovyanGFX-Setup + LVGL-Anbindung
│   ├── ui/                     Screens + wiederverwendbare Widgets
│   │   ├── screen_base.h       Screen-Lifecycle (create/show/refresh/tick)
│   │   ├── home_screen.*       3 Touch-Kacheln
│   │   ├── weather_detail_screen.*
│   │   ├── calendar_detail_screen.*
│   │   ├── mvg_screen.*
│   │   └── widgets/            tile_button, back_button
│   ├── api/                    HTTP-Clients (1:1 aus m5stack-dashboard)
│   └── icons/                  prozedurale LVGL-Canvas-Icons
├── scripts/
│   ├── deploy.sh / deploy.cmd  Build + Flash
│   └── build.sh / build.cmd    Nur Build
└── docs/                       Migrationshistorie (SPEC-old, PLAN, TODO)
```

## Utilities

- **`text_utils.h`** – Transliteriert deutsche Umlaute und Sonderzeichen von UTF-8 nach ASCII, damit LVGL-Labels Sonderzeichen korrekt darstellen können.
- **`date_utils.h`** – Datumsformatierung ohne NTP-Abhängigkeit. Formatiert Zeitstempel aus den API-Responses in lesbare deutsche Strings.
- **`theme.h`** – Dark-Mode-Farbschema für LVGL: sehr dunkler Hintergrund (`#000000`/`#0B0B0D`), heller Text, dezente Akzentfarben pro Kachel.

## Icons

Icons werden prozedural als LVGL-Canvas-Objekte gezeichnet (keine externen Bitmap-Dateien). Das `src/icons/`-Verzeichnis enthält Funktionen, die Icon-Shapes direkt im Code erzeugen – u.a. Wetter-Icons (Sonne, Wolken, Regen), U-Bahn/S-Bahn-Symbole, Kalender-Icon, Zurück-Pfeil und Retry-Icon. Vorteil: Kein Nachladen von SD-Karte, keine .bin-Abhängigkeit, Icon-Pixel sind im Flash gespeichert.

## Referenzprojekt

Vorgängerprojekt mit M5Stack Core: <https://github.com/skoelle/m5stack-dashboard>
