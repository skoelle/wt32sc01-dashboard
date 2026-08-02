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
│   ├── deploy.sh               Build + Flash
│   └── build.sh                Nur Build
└── docs/                       Migrationshistorie (SPEC-old, PLAN, TODO)
```

## Referenzprojekt

Vorgängerprojekt mit M5Stack Core: <https://github.com/skoelle/m5stack-dashboard>
