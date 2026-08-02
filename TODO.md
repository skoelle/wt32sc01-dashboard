# TODO – WT32-SC01 Plus Dashboard

_Letztes Update: 2026-08-02_ · Details/Begründungen siehe `PLAN.md`, Fachanforderungen siehe `SPEC.md`

## Phase 0 – Setup

- [ ] Neues GitHub-Repository anlegen (Vorschlag: `wt32sc01-dashboard`)
- [ ] `git init` lokal, Remote verknüpfen, erster Commit (leeres PlatformIO-Grundgerüst)
- [ ] `platformio.ini` neu anlegen: Board `esp32-s3`, Framework `arduino`, PSRAM aktivieren
- [ ] LovyanGFX und LVGL (v9) als Lib-Dependencies eintragen
- [ ] `.gitignore` aus `m5stack-dashboard` übernehmen (inkl. `include/secrets.h`, `.pio/`)
- [ ] `include/secrets.h.example` aus Original übernehmen, `include/secrets.h` lokal anlegen (nicht committen)

## Phase 1 – Display & Touch

- [ ] Exakte Pinbelegung des konkret vorliegenden WT32-SC01-Plus-Boards verifizieren (Display-Datenbus, WR/RD/DC/CS/RST, Backlight, Touch-I2C SDA/SCL/INT)
- [ ] `include/board_pins.h` mit verifizierten Pin-Defines anlegen
- [ ] `src/display/display_setup.cpp/.h`: LovyanGFX-Panel-Konfiguration für ST7796 (8-Bit-Parallel/i80) erstellen
- [ ] Display-Rotation auf Portrait (320×480) erzwingen
- [ ] Touchcontroller FT6336U initialisieren (I2C) und in LVGL als Input-Device registrieren
- [ ] LVGL-Tick-Timer (`lv_tick_inc`) und Haupt-Loop (`lv_timer_handler`) einbauen
- [ ] Testscreen mit einem einzelnen antippbaren Button bauen, Touch-Koordinaten/Kalibrierung verifizieren

## Phase 2 – API-Schicht portieren

- [ ] `src/api/http_client.h/cpp` aus Original-Repo in neues Repo kopieren, Includes/Namespaces anpassen
- [ ] `src/api/weather_api.h/cpp` kopieren, gegen Endpoint `http://docker-host-pve.fritz.box:3088/api/weather` testen
- [ ] `src/api/calendar_api.h/cpp` kopieren, gegen Endpoint `http://docker-host-pve.fritz.box:8077/api/events` testen
- [ ] `src/api/departures_api.h/cpp` kopieren, gegen Endpoint `http://docker-host-pve.fritz.box:8078/api/departures` testen
- [ ] Alle drei API-Clients unabhängig vom UI verifizieren (Serial-Log: Parsing korrekt, Felder vollständig)
- [ ] WLAN-Verbindungsaufbau (Setup-Routine) aus Original übernehmen/anpassen

## Phase 3 – Wiederverwendbare UI-Bausteine

- [ ] `src/ui/widgets/tile_button.h/cpp`: LVGL-Kachel-Komponente (Icon + Titel + optionaler Wert/Vorschau-Text), touch-tauglich groß dimensioniert
- [ ] `src/ui/widgets/back_button.h/cpp`: fixer Zurück-Button unten links, auf allen Detailseiten wiederverwendbar
- [ ] Gemeinsames Farbschema/Theme (Dark Mode, Akzentfarben pro Kategorie) als LVGL-Style-Konstanten definieren
- [ ] `src/ui/screen_base.h`: gemeinsame Screen-Lifecycle-Abstraktion (create/show/hide/refresh) für LVGL

## Phase 4 – Home-Screen

- [ ] `src/ui/home_screen.h/cpp`: Layout mit Wetter-Kachel (groß, oben), Kalender-Kachel (groß, mit 2 Terminen), MVG-Kachel (klein, ohne Vorschau)
- [ ] Fachlogik aus Original `home_screen.cpp` übernehmen: Temperatur/Icon/Beschreibung-Aufbereitung, Regen-Hinweis-Schwellwert-Logik, Termin-Formatierung (`all_day` vs. Uhrzeit)
- [ ] Touch-Handler: Tap auf Wetter-Kachel → Wetter-Detail, Tap auf Kalender-Kachel → Kalender-Detail, Tap auf MVG-Kachel → MVG-Seite
- [ ] Regen-Hinweis-Badge auf Wetter-Kachel einbauen
- [ ] Refresh-Timer 10 Minuten (Wetter + Kalender) einbauen

## Phase 5 – Detailseiten

- [ ] `src/ui/weather_detail_screen.h/cpp`: scrollbare Stundenliste (Zeit, Temperatur, Icon, Regenwahrscheinlichkeit), Fachlogik aus Original übernehmen
- [ ] `src/ui/calendar_detail_screen.h/cpp`: scrollbare Liste aller 10 Termine, Fachlogik aus Original übernehmen
- [ ] `src/ui/mvg_screen.h/cpp`: scrollbare Liste aller Abfahrten (Linie, Ziel, Zeit, Verspätung, Ausfall-Hinweis), Fachlogik aus Original übernehmen
- [ ] `back_button`-Widget auf allen drei Detailseiten unten links einbinden und verdrahten
- [ ] Refresh-Timer 1 Minute für MVG-Seite einbauen

## Phase 6 – Icons & Feinschliff

- [ ] `icons.h` aus Original sichten: welche Icons sind direkt übernehmbar, welche müssen in höherer Auflösung neu exportiert werden
- [ ] Fehlende/größere Icon-Varianten neu rendern (RGB565-Arrays), Icon-Set vervollständigen (inkl. Zurück-Pfeil-Icon, das es im Original nicht gab)
- [ ] 5-Minuten-Inaktivitäts-Timer (automatischer Rücksprung zur Hauptseite) implementieren
- [ ] Fehlerzustände (API nicht erreichbar) auf allen Screens mit Retry-Icon/Text umsetzen, manueller Retry per Tap
- [ ] Visuelle Feinabstimmung: Schriftgrößen, Abstände, Kachel-Farben gegen Referenzbild/SPEC.md prüfen

## Phase 7 – Deploy & Doku

- [ ] `scripts/deploy.sh` aus Original übernehmen, Board-Flag/Baudrate für WT32-SC01 Plus anpassen
- [ ] Testen: Build + Flash über `deploy.sh` ohne Parameter (Auto-Port) und mit explizitem Port
- [ ] `README.md` schreiben (Setup-Anleitung, Verweis auf `m5stack-dashboard` als Referenzprojekt)
- [ ] End-to-End-Test auf echter Hardware: alle 4 Screens, alle Touch-Interaktionen, beide Refresh-Intervalle, Inaktivitäts-Rücksprung, Fehlerfall (WLAN/API trennen)
- [ ] Repo aufräumen (nicht benötigte PlatformIO-Boilerplate entfernen), finalen Commit/Tag setzen

## Blocker / braucht Klärung

- [ ] Original-Quelltext (`main.cpp`, Screen-`.cpp`-Dateien, `icons.h`) muss noch im Detail eingesehen werden (GitHub-Tool lieferte bisher nur Struktur, keinen Volltext) – vor Phase 2/4/5/6 nachholen, um exakte Portier-Aufwände zu bestätigen
- [ ] Pinbelegung WT32-SC01 Plus am konkreten Board verifizieren (Blocker für Phase 1)
- [ ] Name des neuen Repositories final bestätigen
