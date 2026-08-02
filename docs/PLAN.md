# PLAN – WT32-SC01 Plus Dashboard (Migration von m5stack-dashboard)

_Letztes Update: 2026-08-02_

Referenzprojekt: [skoelle/m5stack-dashboard](https://github.com/skoelle/m5stack-dashboard) (Struktur bekannt, Quelltext-Details noch nicht vollständig eingesehen – siehe Abschnitt 6 „Offene Punkte“).

Ziel: Neues, eigenständiges Repository `wt32sc01-dashboard`, das fachlich dieselbe Funktionalität liefert wie das M5Stack-Projekt (siehe `SPEC.md`), aber mit Touch-UI im Hochformat auf dem WT32-SC01 Plus. **So viel wie möglich vom bestehenden Code wird übernommen bzw. 1:1 portiert**, nur die Display-/Eingabe-Schicht wird neu gebaut.

## 1. Migrationsprinzip: Was bleibt, was wird neu gebaut

Die bestehende Architektur trennt bereits sauber zwischen **Datenschicht** (API-Clients, JSON-Parsing) und **Darstellungsschicht** (Screens, Buttons). Diese Trennung ist der Hebel für die Migration:

| Modul (Original) | Migrationsstrategie | Begründung |
|---|---|---|
| `src/api/http_client.h/cpp` | **1:1 übernehmen**, ggf. minimal anpassen (Timeout/Retry-Verhalten prüfen) | Reiner HTTP-GET-Wrapper, hardwareunabhängig, gleiche APIs im gleichen Netz |
| `src/api/weather_api.h/cpp` | **1:1 übernehmen** | Reines JSON-Parsing gegen unveränderten Endpoint, keine Display-Abhängigkeit |
| `src/api/calendar_api.h/cpp` | **1:1 übernehmen** | Gleiche Begründung wie Wetter |
| `src/api/departures_api.h/cpp` | **1:1 übernehmen** | Gleiche Begründung wie Wetter |
| `src/icons/icons.h` (RGB565-Bitmaps) | **Teilweise übernehmen, teilweise neu rendern** | Farbkonzept/Symbolik bleibt, aber Icons werden für größeres Display in größerer Auflösung neu exportiert (siehe Task 3) |
| `src/screens/screen_base.h` | **Konzept übernehmen, Implementierung neu** | Abstraktion "Screen mit Lifecycle" bleibt sinnvoll, aber Rendering-API wechselt von M5GFX-Direktzeichnung auf LVGL-Widgets |
| `src/screens/home_screen.h/cpp` | **Fachlogik übernehmen, UI neu bauen** | Datenaufbereitung (welche Werte werden wie angezeigt) bleibt, Layout wird von Button-Screen zu Touch-Kacheln (siehe SPEC.md Abschnitt 5.1) |
| `src/screens/weather_detail_screen.h/cpp` | **Fachlogik übernehmen, UI neu bauen** | Liste wird scrollbar statt starr, Zurück-Button neu (unten links) |
| `src/screens/calendar_detail_screen.h/cpp` | **Fachlogik übernehmen, UI neu bauen** | Gleiche Begründung |
| `src/screens/mvg_screen.h/cpp` | **Fachlogik übernehmen, UI neu bauen** | Gleiche Begründung |
| `src/main.cpp` | **Neu schreiben** | Boot-Flow, Setup/Loop müssen an LVGL-Tick-Handler, Touch-Init und Display-Rotation angepasst werden |
| `platformio.ini` | **Neu schreiben** | Anderes Board (`esp32-s3`), andere Libs (LovyanGFX + LVGL9 statt M5Stack-Lib), andere Build-Flags |
| Button-Navigation (A/B/C) | **Entfällt komplett** | Ersetzt durch Touch-Event-Handler auf Kacheln/Buttons |
| `.gitignore`, `secrets.h.example` | **1:1 übernehmen** | Muster ist hardwareunabhängig |

**Kurzfassung**: Die komplette `src/api/`-Schicht wandert praktisch unverändert ins neue Repo. Die `src/screens/`-Schicht wird pro Screen in zwei Teile zerlegt: Datenaufbereitung (übernehmen) und Rendering (neu, LVGL-basiert). Nur `main.cpp`, `platformio.ini` und die neue Touch-/Display-Init sind komplett neuer Code.

## 2. Zielarchitektur (neues Repo)

```
wt32sc01-dashboard/
├── .gitignore                      (übernommen aus m5stack-dashboard)
├── platformio.ini                  (neu: esp32-s3, LovyanGFX, LVGL9)
├── include/
│   ├── secrets.h.example           (übernommen)
│   ├── secrets.h                   (lokal)
│   └── board_pins.h                (neu: WT32-SC01-Plus-Pinout)
├── src/
│   ├── main.cpp                    (neu)
│   ├── display/
│   │   └── display_setup.cpp/.h    (neu: LovyanGFX-Config, Portrait-Rotation, Touch-Init)
│   ├── api/                        (portiert, siehe Abschnitt 1)
│   │   ├── http_client.h/cpp
│   │   ├── weather_api.h/cpp
│   │   ├── calendar_api.h/cpp
│   │   └── departures_api.h/cpp
│   ├── ui/
│   │   ├── screen_base.h           (Konzept portiert, LVGL-Basis neu)
│   │   ├── home_screen.h/cpp       (Datenlogik portiert, Kachel-UI neu)
│   │   ├── weather_detail_screen.h/cpp   (Datenlogik portiert, Listen-UI neu)
│   │   ├── calendar_detail_screen.h/cpp  (Datenlogik portiert, Listen-UI neu)
│   │   ├── mvg_screen.h/cpp        (Datenlogik portiert, Listen-UI neu)
│   │   └── widgets/
│   │       ├── tile_button.h/cpp   (neu: wiederverwendbare Touch-Kachel)
│   │       └── back_button.h/cpp   (neu: wiederverwendbarer Zurück-Button unten links)
│   └── icons/
│       └── icons.h                 (teilweise übernommen, teilweise neu gerendert – siehe Task 3)
├── scripts/
│   └── deploy.sh                   (übernommen, ggf. Board-Flag angepasst)
└── README.md                       (neu, verweist auf Referenzprojekt)
```

## 3. Vorgehen in Phasen

### Phase 0 – Setup
Neues Repo anlegen, Grundgerüst (PlatformIO-Projekt für `esp32-s3`) erstellen, LovyanGFX + LVGL9 als Dependencies einbinden, `.gitignore`/`secrets.h.example` aus Original übernehmen.

### Phase 1 – Display & Touch zum Laufen bringen
`board_pins.h` mit verifizierter Pinbelegung, `display_setup.cpp` mit LovyanGFX-Panel-Konfiguration (ST7796, 8-Bit-Parallel), Portrait-Rotation erzwingen (320×480), Touch-Treiber (FT6336U) initialisieren, LVGL-Tick/Loop einbinden. Erfolgskriterium: ein einfaches Testrechteck lässt sich per Touch anwählen.

### Phase 2 – API-Schicht portieren
`src/api/*` unverändert (oder mit minimalen Anpassungen an neue Ordnerstruktur/Includes) ins neue Repo kopieren, gegen die drei bestehenden Endpunkte (Wetter/Kalender/MVG) testen – unabhängig vom UI, z.B. über Serial-Log-Ausgabe verifizieren, dass JSON korrekt geparst wird.

### Phase 3 – Wiederverwendbare UI-Bausteine
`tile_button` (große antippbare Kachel mit Icon/Titel/Wert) und `back_button` (fixe Position unten links) als LVGL-Komponenten bauen, da beide auf mehreren Screens verwendet werden.

### Phase 4 – Home-Screen
Wetter-Kachel, Kalender-Kachel (mit 2 Terminen als Vorschau), kleine MVG-Kachel ohne Vorschau; Touch-Handler pro Kachel verlinken auf jeweilige Detailseite; Regen-Hinweis-Badge auf Wetter-Kachel.

### Phase 5 – Detailseiten
Wetter-Detail (scrollbare Stundenliste), Kalender-Detail (scrollbare 10-Termine-Liste), MVG-Seite (scrollbare Abfahrtsliste) – jeweils mit `back_button` unten links; Fachlogik (Datenaufbereitung/Formatierung) so weit möglich 1:1 aus den Original-Screens übernehmen.

### Phase 6 – Icons & Feinschliff
Icon-Set aus `icons.h` sichten, für größeres Display ggf. in höherer Auflösung neu exportieren (siehe Task unten), Refresh-Timer (10 Min/1 Min) und 5-Minuten-Inaktivitäts-Rücksprung einbauen, Fehlerzustände (Retry-Icon) auf allen Screens testen.

### Phase 7 – Deploy & Doku
`deploy.sh` an Board anpassen (ggf. anderer USB-Chip/Baudrate), README schreiben, finale Tests auf echter Hardware, Repo aufräumen.

## 4. Risiken / Dinge, die beim Portieren zu prüfen sind

- **M5GFX- vs. LovyanGFX-API-Unterschiede**: Falls die Original-Screens direkt M5GFX-Zeichenbefehle nutzen (statt nur Daten aufzubereiten), lässt sich die UI-Logik nicht 1:1 kopieren, sondern muss pro Screen als LVGL-Widget-Baum neu formuliert werden – die Datenaufbereitung (Strings, Werte, Icon-Auswahl) bleibt aber portierbar.
- **Icon-Format**: RGB565-Arrays aus `icons.h` sind ggf. für 2.0"-Auflösung dimensioniert und müssen für die größere, höher aufgelöste Darstellung auf dem 3.5"-Display neu erzeugt werden (skalieren oder aus Original-Vektoren neu rendern).
- **Speicher/PSRAM**: LVGL-Framebuffer für 320×480 (bzw. Teilbuffer) muss ins PSRAM des ESP32-S3-WROVER gelegt werden – Buffer-Größen in `display_setup.cpp` entsprechend konfigurieren.
- **Pin-Variante**: WT32-SC01-Plus-Board-Revisionen unterscheiden sich leicht in der Pinbelegung – vor Phase 1 anhand des konkreten Boards verifizieren.

## 5. Nicht-Ziele

- Kein Rewrite der Backend-APIs (Wetter/Kalender/MVG) – bleiben unverändert.
- Keine Rückwärtskompatibilität zum M5Stack-Repo (kein gemeinsamer Code, kein Git-Submodule/Fork).
- Keine Sprachumschaltung/Mehrsprachigkeit, keine neuen Datenquellen – Funktionsumfang bleibt wie in `SPEC.md` beschrieben.

## 6. Offene Punkte

- Der genaue Inhalt von `main.cpp`, den Screen-`.cpp`-Dateien und `icons.h` im Original-Repo konnte technisch noch nicht vollständig ausgelesen werden (GitHub-Tool lieferte bislang nur Dateistruktur/Metadaten, keinen Volltext). Die konkreten Portier-Schritte in Phase 2–6 sollten beim Start der Umsetzung anhand des tatsächlichen Codes verifiziert/verfeinert werden.
- Exakte Pinbelegung des WT32-SC01 Plus (siehe SPEC.md Abschnitt 11).
- Name/Ort des neuen Repositories final bestätigen.
