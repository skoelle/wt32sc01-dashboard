# AGENTS.md

## Projektübersicht

WT32-SC01 Plus Dashboard für ESP32-S3 (3.5" kapazitiver Touch-IPS-Display, 320×480 Hochformat). Zeigt Wetter, Kalendertermine und MVG-Abfahrten an, gesteuert ausschließlich über Touch.

**Tech-Stack:**
- PlatformIO mit Arduino-Framework
- LovyanGFX als Display-/Touch-Treiber
- LVGL 9 für UI-Widgets
- ESP32-S3 mit PSRAM

## Wichtige Dateien

- `platformio.ini` - Build-Konfiguration, Board-Einstellungen, Abhängigkeiten
- `include/board_pins.h` - Pinbelegung für WT32-SC01 Plus
- `include/secrets.h` - WLAN-Zugangsdaten (nicht im Repo)
- `include/theme.h` - Dark-Mode-Farbschema
- `src/main.cpp` - Screen-Controller, Navigation, Inaktivitäts-Timer
- `src/ui/screen_base.h` - Screen-Lifecycle (create/show/refresh/tick)
- `src/ui/home_screen.*` - Hauptseite mit 3 Touch-Kacheln
- `src/ui/weather_detail_screen.*` - Wetter-Detailseite
- `src/ui/calendar_detail_screen.*` - Kalender-Detailseite
- `src/ui/mvg_screen.*` - MVG-Abfahrtsseite
- `src/api/` - HTTP-Clients für Wetter, Kalender, MVG
- `src/icons/` - Prozedurale LVGL-Canvas-Icons
- `scripts/deploy.sh` / `deploy.cmd` - Build + Flash
- `scripts/build.sh` / `build.cmd` - Nur Build

## Build & Deploy

```bash
# Build + Flash (automatische Port-Erkennung)
./scripts/deploy.sh

# Build + Flash mit explizitem Port
./scripts/deploy.sh /dev/ttyUSB0

# Nur Build (ohne Flash) - Erfordert virtuelle Umgebung
.venv-platformio/bin/pio run

# Serieller Monitor
pio device monitor
```

## Projektstruktur

```
/
├── platformio.ini              # esp32-s3, LovyanGFX + LVGL9
├── lv_conf.h                   # LVGL-Konfiguration
├── include/
│   ├── board_pins.h            # WT32-SC01-Plus-Pinout
│   ├── secrets.h.example       # Platzhalter für WLAN/API-URLs
│   ├── theme.h                 # Dark-Mode-Farbschema (LVGL)
│   ├── text_utils.h            # UTF-8->ASCII-Transliteration
│   └── date_utils.h            # Datumsformatierung (kein NTP)
├── src/
│   ├── main.cpp                # Screen-Controller + Inaktivitäts-Timer
│   ├── display/                # LovyanGFX-Setup + LVGL-Anbindung
│   ├── ui/                     # Screens + wiederverwendbare Widgets
│   │   ├── screen_base.h       # Screen-Lifecycle (create/show/refresh/tick)
│   │   ├── home_screen.*       # 3 Touch-Kacheln
│   │   ├── weather_detail_screen.*
│   │   ├── calendar_detail_screen.*
│   │   ├── mvg_screen.*
│   │   └── widgets/            # tile_button, back_button
│   ├── api/                    # HTTP-Clients (1:1 aus m5stack-dashboard)
│   └── icons/                  # prozedurale LVGL-Canvas-Icons
├── scripts/
│   ├── deploy.sh / deploy.cmd  # Build + Flash
│   └── build.sh / build.cmd    # Nur Build
└── docs/                       # Migrationshistorie (SPEC-old, PLAN, TODO)
```

## Screen-Architektur

Jeder Screen implementiert die `Screen`-Struktur aus `screen_base.h`:
- `create_fn()` - Baut UI-Komponenten einmalig beim ersten Aufruf
- `refresh_fn()` - Holt neue Daten und aktualisiert Widgets
- `tick_fn()` - Wird jeden Loop-Durchlauf aufgerufen (für Timer)
- `show()` - Zeigt den Screen (erstellt ihn bei Bedarf)

Navigation erfolgt über `navigate(ScreenId)` Funktion in `main.cpp`.

## API-Endpunkte

Alle APIs liefern JSON per HTTP GET (URLs in `include/secrets.h` konfigurierbar):
- Wetter: `/api/weather`
- Kalender: `/api/events`
- MVG: `/api/departures`

## Refresh-Intervalle

| Seite | Intervall |
|-------|-----------|
| Hauptseite (Wetter + Kalender) | 10 Minuten |
| MVG-Abfahrtsseite | 1 Minute |
| Detailseiten | Folgen Hauptseiten-Intervall |

## Coding-Konventionen

- C++ mit Arduino-Framework
- Keine externen Libraries außer: LovyanGFX, LVGL 9, ArduinoJson
- Screen-Dateien: `*_screen.h` + `*_screen.cpp`
- API-Dateien: `*_api.h` + `*_api.cpp`
- Widgets in `src/ui/widgets/`
- Icons als prozedurale LVGL-Canvas-Objekte (keine Bitmaps)
- Dark-Mode-Farbschema mit `#000000`/`#0B0B0D` Hintergrund
- Touch-taugliche Mindestgröße: ≥ 80-100 px Höhe
- UTF-8-Transliteration für deutsche Umlaute (`text_utils.h`)

## Fehlerbehandlung

- Bei nicht erreichbarer API: Fehleranzeige auf Screen/Kachel
- Retry automatisch beim nächsten Refresh-Intervall
- Manueller Retry durch erneutes Antippen der Kachel
- Kein "letzter bekannter Wert" über Fehlerzustand hinaus

## Referenzprojekt

Vorgängerprojekt mit M5Stack Core: <https://github.com/skoelle/m5stack-dashboard>
(Funktionalität übernommen, komplett neu geschrieben für WT32-SC01 Plus)