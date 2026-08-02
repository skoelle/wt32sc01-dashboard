# WT32-SC01 Plus – Wetter/Kalender/MVG Dashboard (Touch)

_Letztes Update: 2026-08-02_

## 1. Ziel

Ein WT32-SC01 Plus (ESP32-S3, 3.5" kapazitiver Touch-IPS-Display) zeigt Wetter, Kalendertermine und MVG-Abfahrten an – funktional analog zum bestehenden [M5Stack-Dashboard](https://github.com/skoelle/m5stack-dashboard), aber als **eigenständiges, komplett neu geschriebenes Projekt** in einem **neuen, eigenen Git-Repository**. Es wird kein Code aus dem M5Stack-Projekt übernommen; lediglich das UI-/API-Konzept und die Backend-Endpunkte werden wiederverwendet.

Kernunterschiede zum M5Stack:

- **Touch statt Buttons**: Navigation ausschließlich über Touch-Buttons auf dem Screen, keine physischen Tasten.
- **Deutlich größeres Display** (3.5" statt 2.0"), dadurch großzügigere, besser lesbare Darstellung von Listen (Abfahrten, Termine, Stundenvorhersage).
- **Hochformat (Portrait)** statt Querformat, damit sich Listen (MVG-Abfahrten, Termine, stündliche Vorhersage) besser vertikal darstellen lassen.
- **Home-Screen als Buttons/Kacheln**: Wetter- und Kalenderbereich sind selbst antippbare Kacheln, die direkt zur jeweiligen Detailseite führen (kein separater Toggle-Button nötig).

## 2. Hardware

- **Gerät**: WT32-SC01 Plus, ESP32-S3-WROVER (Dual-Core Xtensa LX7, PSRAM)
- **Display**: 3.5" IPS, Treiber-IC ST7796UI, physische Auflösung 480×320 (Querformat ab Werk), Ansteuerung über 8-Bit-Parallel-Interface (i80/8080)
- **Software-Orientierung**: Hochformat (Portrait) → logische Auflösung **320×480** (Breite × Höhe), per Display-Rotation im Code erzwungen
- **Touch**: Kapazitiver Touchcontroller FT6336U, I2C, Single-Touch ausreichend für diese App
- **Sonstige Peripherie am Board**: Lautsprecher, SD-Karte, RS485 – werden für dieses Projekt nicht benötigt
- **Netzwerk**: WLAN (Heimnetz, Zugriff auf `*.fritz.box` Hosts, gleiches Netz wie M5Stack)

> Hinweis: Pinbelegung (Display-Datenleitungen, Touch-I2C, Backlight, Reset) ist je nach Board-Revision leicht unterschiedlich dokumentiert und wird beim Projektstart anhand des konkret vorliegenden Boards verifiziert und in einer eigenen `board_pins.h` fixiert.

## 3. Toolchain

- **Build-System**: PlatformIO (kein Arduino IDE), analog zum M5Stack-Projekt
- **Grafik-Stack**: LovyanGFX als Display-/Touch-Treiber in Kombination mit LVGL (Version 9) für UI-Widgets, Touch-Buttons, Listen und Scroll-Verhalten – bewährte Kombination für das WT32-SC01 Plus
- **Deployment**: Eigenes Deploy-Skript `scripts/deploy.sh`, das ausschließlich **Build + Flash** durchführt (kein automatisches Öffnen des seriellen Monitors, kein Zusatzschritt danach). USB-Port wird standardmäßig automatisch erkannt, kann aber optional als Parameter übergeben werden (z.B. `./deploy.sh /dev/ttyUSB0`)
- **WLAN-Zugangsdaten**: Fest im Code, ausgelagert in `include/secrets.h`, per `.gitignore` vom Repo ausgeschlossen. Ein `secrets.h.example` mit Platzhaltern wird eingecheckt
- **Zeitsynchronisation**: Keine eigene NTP-Sync im Gerät. Alle Zeitangaben werden 1:1 so übernommen und dargestellt, wie sie von den APIs geliefert werden (keine relative Umrechnung wie "in 20 Minuten")
- **Versionskontrolle**: Es wird von Anfang an ein **neues, eigenes Git-Repository** angelegt (`git init` + eigener Remote, z.B. `wt32sc01-dashboard`). Kein Fork und kein geteilter Code mit dem M5Stack-Repo – lediglich als Referenz/Vorbild verlinkt

## 4. Datenquellen (APIs)

Identisch zum M5Stack-Projekt, gleiche Backend-Services im lokalen Netz, JSON per HTTP GET. Response-Formate werden **1:1 übernommen**, keine Änderungen am Backend nötig.

### 4.1 Wetter-API

- **Endpoint**: `/api/weather` (konfigurierbar in `include/secrets.h`)
- Liefert `current` (aktuelles Wetter: `temperature`, `symbol`, `description`, `emoji`) und `forecast` (stündliche Vorhersage inkl. `precipitation.probability` und `precipitation.type`)
- `symbol` (z.B. `mo____`, `mb____`, `wb____`) ist Basis für die Icon-Auswahl

### 4.2 Kalender-API

- **Endpoint**: `/api/events` (konfigurierbar in `include/secrets.h`)
- Liefert die nächsten 10 Termine, serverseitig chronologisch sortiert (`summary`, `start_at`, `end_at`, `all_day`, `status`, u.a.)
- Für den Home-Screen werden die ersten 2 Einträge verwendet, für die Detailseite alle 10
- `all_day`-Termine werden anders dargestellt (nur Datum statt Uhrzeit); Zeiten werden unverändert übernommen

### 4.3 MVG-Abfahrten-API

- **Endpoint**: `/api/departures` (konfigurierbar in `include/secrets.h`)
- Liefert eine ungefilterte Liste aller Abfahrten (beide Stationen, U-Bahn/S-Bahn gemischt, Reihenfolge wie geliefert): `station`, `type`, `icon`, `line`, `destination`, `time_str`, `delay_min`, `cancelled`, `messages`
- `time_str` wird direkt übernommen, keine eigene Zeitberechnung

## 5. Screens (Hochformat, 320×480)

### 5.1 Hauptseite (Home)

Ruhezustand nach Boot. Besteht aus großen, antippbaren **Kacheln/Touch-Buttons**, angeordnet untereinander (Portrait-Layout), im Kachel-Stil ähnlich der Referenzoptik (siehe Abschnitt 7):

- **Wetter-Kachel** (groß, oben): aktuelle Temperatur, Icon, Beschreibung, ggf. Regen-Hinweis-Badge. Antippen → Wetter-Detailseite
- **Kalender-Kachel** (groß, darunter): zeigt die nächsten 2 Termine (Summary + Datum/Uhrzeit, `all_day` gesondert markiert) direkt auf der Kachel als Vorschau. Antippen → Kalender-Detailseite
- **MVG-Kachel** (klein, unten, **ohne** Abfahrten-Vorschau – nur Icon/Label "MVG" bzw. "Abfahrten"): Antippen → MVG-Abfahrtsseite

Regen-Hinweis (weicher Schwellwert): Sobald irgendein Eintrag der nächsten 8 Vorhersage-Stunden `precipitation.type == "rain"` mit `probability > 0` enthält, erscheint ein Regen-Hinweis-Icon/Badge auf der Wetter-Kachel.

Refresh: alle 10 Minuten (Wetter + Kalender neu abrufen)

### 5.2 Wetter-Detailseite

- Aktuelles Wetter ausführlicher als auf der Home-Kachel
- Stundenweise Vorhersage aus `forecast` als vertikal scrollbare Liste (Zeit, Temperatur, Icon, Regenwahrscheinlichkeit) – dank Portrait-Format und größerem Display deutlich übersichtlicher als auf dem M5Stack
- **Zurück-Button unten links** → zurück zur Hauptseite

### 5.3 Kalender-Detailseite

- Liste aller 10 Termine aus `events` als vertikal scrollbare Liste (nicht nur die ersten 2 wie auf der Home-Kachel)
- **Zurück-Button unten links** → zurück zur Hauptseite

### 5.4 MVG-Abfahrtsseite

- Liste aller Abfahrten aus `departures`, ohne Filterung nach Station oder Linie, als vertikal scrollbare Liste (Linie, Ziel, Zeit, Verspätung, ggf. Ausfall-Hinweis)
- **Zurück-Button unten links** → zurück zur Hauptseite
- Kein Background-Refresh (nur beim Betreten der Seite und via tick() alle 60 Sekunden)

## 6. Navigation (Touch)

Keine physischen Buttons – ausschließlich Touch-Bedienung:

| Aktion | Funktion |
|---|---|
| Tap auf Wetter-Kachel (Home) | Öffnet Wetter-Detailseite |
| Tap auf Kalender-Kachel (Home) | Öffnet Kalender-Detailseite |
| Tap auf MVG-Kachel (Home) | Öffnet MVG-Abfahrtsseite |
| Tap auf Zurück-Button (unten links, auf jeder Detailseite) | Zurück zur Hauptseite |

Zusätzlich: Automatischer Rücksprung zur Hauptseite nach 5 Minuten Inaktivität (keine Touch-Eingabe), unabhängig davon, auf welcher Seite man sich gerade befindet.

## 7. UI- und Icon-Konzept

Gleiches Grundprinzip wie beim M5Stack (hochwertig, farbig, dunkles Farbschema), zusätzlich angelehnt an die im Referenzbild gezeigte **Kachel-Optik** (große, abgerundete Rechtecke mit Icon + Label + kleinem Zusatzwert), übertragen auf ein Hochformat-Dashboard:

- **Farbschema "iPhone Dark Mode"-Look**: Sehr dunkles Schwarz (`#000000`/`#0B0B0D`) als Hintergrund, weißer/heller Text (`#FFFFFF`/`#F2F2F7`) als Basis. Jede Home-Kachel bekommt einen eigenen, dezenten Akzentton als Kachel-Hintergrund (z.B. gedämpftes Blau für Wetter, eigene Akzentfarbe für Kalender, an MVV-Linienfarben angelehnte Töne für MVG), ähnlich den farbigen Funktionskacheln im Referenzbild, aber ruhiger/dunkler
- **Größere, touch-taugliche Kacheln**: Mindestgröße der Home-Kacheln so bemessen, dass sie bequem mit dem Finger treffbar sind (Richtwert ≥ 80–100 px Höhe bei 320 px Breite); Zurück-Button ebenfalls als große, gut treffbare Fläche unten links
- **Eigene prozedurale Icons** als LVGL-Canvas-Objekte (keine externen Bitmap-Dateien), Icon-Set: Sonne/klar, bewölkt, Regen, Nacht-Varianten (Basis: `symbol`-Feld), U-Bahn-Symbol, S-Bahn-Symbol, Kalender-Symbol, Regen-Hinweis-Symbol, Zurück-Pfeil, Retry-/Fehler-Symbol
- **Layout-Prinzipien**: Klare visuelle Hierarchie (große Temperatur/Uhrzeit, kleinere Nebeninfos), hoher Kontrast schwarz/weiß als Basis, großzügiger als beim M5Stack dank 3.5"-Display und Portrait-Ausrichtung, dezente Akzentfarben statt vieler bunter Flächen
- **Typografie**: Deutlich größere, gut lesbare Schriftgrößen als beim M5Stack (mehr Platz vorhanden), wichtige Werte (Temperatur, Abfahrtszeit) groß und fett gegenüber Nebeninfos

## 8. Fehlerbehandlung

- Bei nicht erreichbarer API: Fehleranzeige auf dem betroffenen Screen bzw. der betroffenen Kachel (Retry-Icon + kurzer Text wie "Keine Verbindung"). Letzte gültige Daten bleiben erhalten (außer bei MVG: nur aktuelle Daten, da kein Background-Refresh)
- **Retry-Auslöser**: Automatisch beim nächsten regulären Refresh-Intervall, zusätzlich manuell durch erneuten Tap auf die betroffene Kachel bzw. durch Zurück- und wieder-Reinnavigieren in den Screen

## 9. Refresh-Intervalle

| Seite/Datenquelle | Intervall | Background-Refresh |
|---|---|---|
| Hauptseite (Wetter + Kalender) | 10 Minuten | Ja (data_manager) |
| MVG-Abfahrtsseite | 1 Minute | Nein (nur tick()) |
| Wetter-Detailseite | folgt Hauptseiten-Intervall (10 Minuten), da gleiche Datenquelle | Ja (data_manager) |
| Kalender-Detailseite | folgt Hauptseiten-Intervall (10 Minuten), da gleiche Datenquelle | Ja (data_manager) |

## 10. Projektstruktur (PlatformIO, neues Repository)

Eigenständiges, neues Git-Repository (z.B. `wt32sc01-dashboard`), von Anfang an mit `git init` + eigenem Remote, komplett unabhängig vom M5Stack-Repo:

```
/
├── .git/                     (eigenes, neues Repository – kein Fork)
├── .gitignore                (schließt u.a. include/secrets.h, .pio/ aus)
├── platformio.ini            (Board: esp32-s3, LovyanGFX + LVGL9 als Dependencies)
├── include/
│   ├── secrets.h.example     (Platzhalter für WLAN)
│   ├── secrets.h             (lokal, nicht eingecheckt)
│   ├── board_pins.h          (Display-/Touch-Pinbelegung für WT32-SC01 Plus)
│   ├── theme.h               (Dark-Mode-Farbschema)
│   ├── text_utils.h          (UTF-8->ASCII-Transliteration)
│   └── date_utils.h          (Datumsformatierung ohne NTP)
├── src/
│   ├── main.cpp
│   ├── display/              (LovyanGFX-Setup, Rotation/Portrait-Konfiguration)
│   ├── ui/                   (LVGL-Screens: Home, WeatherDetail, CalendarDetail, MVG; Kachel-/Button-Widgets)
│   ├── api/                  (HTTP-Clients für Weather, Calendar, MVG – eigenständig implementiert)
│   └── icons/                (prozedurale LVGL-Canvas-Icons)
├── scripts/
│   ├── deploy.sh / deploy.cmd  (Build + Flash via PlatformIO CLI, kein Monitor)
│   └── build.sh / build.cmd    (Nur Build)
└── README.md                  (verweist als Referenz auf https://github.com/skoelle/m5stack-dashboard)
```

- `deploy.sh` ruft im Kern `pio run --target upload` auf; ohne Parameter automatische Port-Erkennung, mit Parameter (z.B. `./deploy.sh /dev/ttyUSB0`) fester Port. Kein automatisches Starten des seriellen Monitors

## 11. Offene Punkte / Rückfragen

- Exakte Pinbelegung (Display-Datenbus, Touch-I2C-Pins, Backlight) muss anhand des konkret vorliegenden WT32-SC01-Plus-Boards verifiziert werden (Board-Revisionen unterscheiden sich in der Dokumentation leicht)
- Name des neuen GitHub-Repositories ist noch final zu bestätigen (Vorschlag: `wt32sc01-dashboard`)
