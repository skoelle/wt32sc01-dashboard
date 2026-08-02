# M5Stack Core – Wetter/Kalender/MVG Dashboard

_Letztes Update: 2026-08-01 23:16 CEST_

## 1. Ziel

Ein M5Stack Core (ESP32, 2.0" IPS Display, 3 physische Buttons: A, B, C) zeigt Wetter, Kalendertermine und MVG-Abfahrten an. Die Navigation erfolgt ausschließlich über die drei Buttons, es gibt keine Touch-Bedienung. Besonderer Fokus liegt auf einer visuell ansprechenden, **farbigen** UI mit eigenen Bitmap-Icons und einem **dunklen Farbschema**, die trotz des kleinen 2.0" Displays hochwertig aussieht.

## 2. Hardware

- **Gerät**: M5Stack Core (Basic), ESP32-basiert
- **Display**: 2.0" IPS, 320x240 px
- **Eingabe**: 3 Buttons (A, B, C)
- **Netzwerk**: WLAN (Heimnetz, Zugriff auf `*.fritz.box` Hosts)

## 3. Toolchain

- **Build-System**: PlatformIO (kein Arduino IDE)
- **Deployment**: Eigenes Deploy-Skript, das ausschließlich **Build + Flash** durchführt (kein automatisches Öffnen des seriellen Monitors, kein zusätzlicher Schritt danach). USB-Port wird standardmäßig automatisch erkannt (PlatformIO-Standardverhalten), kann aber optional als Parameter/Umgebungsvariable an das Skript übergeben werden, um einen festen Port zu erzwingen (z.B. `./deploy.sh /dev/ttyUSB0`).
- **WLAN-Zugangsdaten**: Fest im Code hinterlegt, aber ausgelagert in eine eigene Datei (z.B. `include/secrets.h` oder `src/secrets.cpp`), die per `.gitignore` vom Git-Repo ausgeschlossen wird. Ein `secrets.h.example` mit Platzhaltern wird stattdessen eingecheckt.
- **Zeitsynchronisation**: Keine eigene NTP-Sync im Gerät. Alle Zeitangaben werden 1:1 so übernommen und dargestellt, wie sie von den APIs geliefert werden (keine relative Umrechnung wie "in 20 Minuten").
- **Versionskontrolle**: Für den ersten Wurf wird noch kein Git-Repository angelegt bzw. initialisiert (kein `git init`, kein Remote). Die Projektstruktur inkl. `.gitignore` und `secrets.h.example` wird trotzdem von Anfang an sauber vorbereitet, damit später jederzeit unkompliziert `git init` + Remote-Verknüpfung nachgeholt werden kann.

## 4. Datenquellen (APIs)

Alle APIs liegen im lokalen Netz und liefern JSON per HTTP GET.

### 4.1 Wetter-API

- **Endpoint**: `http://docker-host-pve.fritz.box:3088/api/weather`
- **Methode**: GET
- **Beispiel-Response**:

```json
{
  "current": {
    "temperature": 20,
    "symbol": "mo____",
    "description": "Klar",
    "emoji": "🌙"
  },
  "forecast": [
    {
      "time": "2026-08-01T23:00:00+02:00",
      "temperature": 20,
      "symbol": "mb____",
      "description": "Bewölkt",
      "emoji": "🌙",
      "precipitation": { "probability": 0.2, "type": "rain" }
    },
    {
      "time": "2026-08-02T00:00:00+02:00",
      "temperature": 20,
      "symbol": "mb____",
      "description": "Bewölkt",
      "emoji": "🌙",
      "precipitation": { "probability": 0.2, "type": "rain" }
    }
  ]
}
```

- `current`: aktuelles Wetter
- `forecast`: stündliche Vorhersage (im Beispiel 8 Einträge), jeder Eintrag enthält u.a. `precipitation.probability` (0.0–1.0) und `precipitation.type` (z.B. `"rain"`)
- `symbol`: interner Wettercode (z.B. `mo____` = klar/Mond, `mb____` = bewölkt/Mond, `wb____` = bewölkt/Tag). Wird als Grundlage für die Auswahl des passenden Bitmap-Icons verwendet (siehe Abschnitt 7)

### 4.2 Kalender-API

- **Endpoint**: `http://docker-host-pve.fritz.box:8077/api/events`
- **Methode**: GET
- **Liefert**: die nächsten 10 Termine (bereits chronologisch sortiert, serverseitig limitiert)
- **Beispiel-Response**:

```json
{
  "events": [
    {
      "id": 205,
      "summary": "Sommerferien",
      "description": "",
      "location": "",
      "start_at": "2026-08-03T00:00:00",
      "end_at": "2026-09-15T00:00:00",
      "all_day": true,
      "status": "CONFIRMED"
    },
    {
      "id": 113,
      "summary": "Zahnarzt Nepomuk",
      "description": "",
      "location": "",
      "start_at": "2026-08-03T08:00:00",
      "end_at": "2026-08-03T09:00:00",
      "all_day": false,
      "status": "CONFIRMED"
    }
  ],
  "count": 10,
  "query_time": "2026-08-01T20:58:37.858934Z"
}
```

- Für die Hauptseite werden die ersten 2 Einträge aus `events` verwendet (nächste 2 Termine)
- `all_day` Termine werden anders dargestellt als Termine mit konkreter Uhrzeit (nur Datum statt Uhrzeit)
- Zeiten (`start_at`, `end_at`) werden unverändert übernommen, keine Umrechnung/Lokalisierung

### 4.3 MVG-Abfahrten-API

- **Endpoint**: `http://docker-host-pve.fritz.box:8078/api/departures`
- **Methode**: GET
- **Beispiel-Response** (gekürzt):

```json
{
  "departures": [
    {
      "station": "Josephsburg, München",
      "type": "UBAHN",
      "icon": "U",
      "line": "U2",
      "destination": "Feldmoching",
      "time_epoch": 1785618120,
      "time_str": "23:02",
      "delay_min": -1,
      "cancelled": false,
      "messages": []
    },
    {
      "station": "Berg am Laim, München",
      "type": "SBAHN",
      "icon": "S",
      "line": "S2",
      "destination": "Pasing",
      "time_epoch": 1785618840,
      "time_str": "23:14",
      "delay_min": 4,
      "cancelled": false,
      "messages": []
    }
  ]
}
```

- **Kein Filter**: Es werden alle zurückgelieferten Abfahrten (beide Stationen, U-Bahn und S-Bahn gemischt) angezeigt, in der Reihenfolge wie von der API geliefert
- `delay_min`: Verspätung in Minuten (kann negativ sein = früher), `cancelled`: Ausfall-Flag
- `time_str` wird direkt übernommen (keine eigene Zeitberechnung)

## 5. Screens

### 5.1 Hauptseite (Home)

Wird nach Boot standardmäßig angezeigt und ist der "Ruhezustand" des Geräts.

Inhalt:
- Aktuelle Temperatur + Icon + Beschreibung (aus `current`)
- Regen-Hinweis (weicher Schwellwert): Sobald irgendein Eintrag der nächsten 8 Vorhersage-Stunden `precipitation.type == "rain"` mit `probability > 0` enthält, wird ein Regen-Hinweis-Icon/Banner angezeigt. Es wird also lieber zu früh als zu spät gewarnt.
- Nächste 2 Kalendertermine (Summary + Datum/Uhrzeit, `all_day` gesondert markiert)

Refresh: alle 10 Minuten (Wetter + Kalender neu abrufen)

### 5.2 Wetter-Detailseite

Inhalt:
- Aktuelles Wetter (ausführlicher als Home)
- Stundenweise Vorhersage aus `forecast` (Zeit, Temperatur, Icon, Regenwahrscheinlichkeit) mit Icons statt Text wo sinnvoll

### 5.3 Kalender-Detailseite

Inhalt:
- Liste aller 10 Termine aus `events` (nicht nur die ersten 2 wie auf der Hauptseite)

### 5.4 MVG-Abfahrtsseite

Inhalt:
- Liste aller Abfahrten aus `departures`, ohne Filterung nach Station oder Linie (Linie, Ziel, Zeit, Verspätung, ggf. Ausfall-Hinweis)

Refresh: jede Minute

## 6. Navigation (Buttons)

| Button | Funktion |
|---|---|
| A | Wechselt zwischen den Detailseiten Wetter und Kalender (Toggle) |
| B | Springt zurück zur Hauptseite |
| C | Öffnet die MVG-Abfahrtsseite |

- Automatischer Rücksprung zur Hauptseite nach 5 Minuten Inaktivität (kein Button-Druck), unabhängig davon, auf welcher Seite man sich gerade befindet

## 7. UI- und Icon-Konzept

Ein zentraler Bestandteil des Projekts ist eine hochwertige, **farbige** und für das kleine Display optimierte Oberfläche, kein reiner Text-Dump.

- **Farbschema "iPhone Dark Mode"-Look**: Primär reines/sehr dunkles Schwarz (`#000000` bzw. `#0B0B0D`-ähnlich) als Hintergrund mit weißem bzw. sehr hellem Text (`#FFFFFF` / `#F2F2F7`) als Basis, ganz im Stil von iOS Dark Mode. Farbe wird bewusst zurückhaltend und dezent als Akzent eingesetzt (z.B. gedämpfte Blau-/Grüntöne für Wetter, eigene dezente Akzentfarbe für Kalender, an echte MVV-Linienfarben angelehnte, aber nicht grelle Töne für U-Bahn/S-Bahn), nicht als große flächige Buntheit
- **Eigene farbige Bitmap-Icons** statt Unicode-Emojis (M5Stack-Displays unterstützen keine nativen Emoji-Fonts). Icons werden als eingebettete Bitmaps (RGB565-Arrays) im Code hinterlegt, nicht als Dateien auf SD-Karte, um Ladezeiten zu vermeiden
- **Icon-Set mindestens für**: Sonne/klar, bewölkt, Regen, Nacht-Varianten (basierend auf dem `symbol`-Feld, z.B. `mo____`, `mb____`, `wb____`), U-Bahn-Symbol, S-Bahn-Symbol, Kalender-Symbol, Warn-/Regen-Hinweis-Symbol, Retry-/Fehler-Symbol – alle farbig statt monochrom
- **Layout-Prinzipien**: Klare visuelle Hierarchie (große Temperatur, kleinere Nebeninfos), hoher Kontrast durch schwarz/weiß als Basis, moderne, aufgeräumte, iOS-inspirierte Optik ohne überladene Screens, dezente Akzentfarben statt vieler bunter Flächen
- **Typografie**: Angepasste, gut lesbare, weiße Schriftgrößen für das 320x240 Display vor schwarzem Hintergrund, wichtige Werte (Temperatur, Abfahrtszeit) deutlich größer und ggf. fett gegenüber Nebeninfos, ganz im Stil moderner iOS-Widgets

## 8. Fehlerbehandlung

- Bei nicht erreichbarer API: Einfache Fehleranzeige auf dem betroffenen Screen (z.B. Retry-Icon + kurzer Text wie "Keine Verbindung")
- **Retry-Auslöser**: Automatisch beim nächsten regulären Refresh-Intervall der jeweiligen Seite (10 Minuten bzw. 1 Minute), zusätzlich manuell durch erneuten Tastendruck auf den Button, der die aktuelle Seite aufruft
- Kein Vorhalten "letzter bekannter Werte" über den Fehlerzustand hinaus gefordert, es genügt die einfache Fehleranzeige bis zum nächsten erfolgreichen Refresh

## 9. Refresh-Intervalle

| Seite/Datenquelle | Intervall |
|---|---|
| Hauptseite (Wetter + Kalender) | 10 Minuten |
| MVG-Abfahrtsseite | 1 Minute |
| Wetter-Detailseite | folgt Hauptseiten-Intervall (10 Minuten), da gleiche Datenquelle |
| Kalender-Detailseite | folgt Hauptseiten-Intervall (10 Minuten), da gleiche Datenquelle |

## 10. Projektstruktur (PlatformIO)

Git-Initialisierung und Remote-Verknüpfung erfolgen bewusst zu einem späteren Zeitpunkt, nicht in diesem ersten Schritt. Die Ordnerstruktur wird aber von Anfang an git-freundlich vorbereitet:

```
/
├── .gitignore              (schließt u.a. include/secrets.h, .pio/ aus – bereits vorbereitet für späteres git init)
├── platformio.ini
├── include/
│   ├── secrets.h.example   (Platzhalter für WLAN, später einzuchecken)
│   └── secrets.h           (lokal, später nicht einzuchecken)
├── src/
│   ├── main.cpp
│   ├── screens/            (Home, WeatherDetail, CalendarDetail, MVG)
│   ├── api/                (HTTP-Clients für Weather, Calendar, MVG)
│   └── icons/              (farbige Bitmap-Icon-Definitionen, RGB565)
├── scripts/
│   └── deploy.sh           (Build + Flash via PlatformIO CLI, USB-Port automatisch erkannt oder optional als Parameter übergeben, kein Monitor)
└── README.md
```

- `deploy.sh` ruft im Kern `pio run --target upload` auf; ohne Parameter wird der Port automatisch erkannt, mit Parameter (z.B. `./deploy.sh /dev/ttyUSB0`) wird ein fester Port erzwungen. Kein automatisches Starten des seriellen Monitors oder weiterer Schritte danach.

## 11. Offene Punkte / Rückfragen

Aktuell keine offenen Punkte mehr, alle wesentlichen Entscheidungen (Toolchain, UI-Stil, Farbschema, Git-Timing, USB-Port-Handling) sind getroffen. Git-Initialisierung und Remote-Repo werden bewusst erst in einem späteren Schritt nachgeholt, sobald der Code lokal funktioniert.
