# Einsatzplaner - Veränderungshistorie
Stand: 7. Oktober 2017

## Version 1.2 - Unveröffentlicht
### Neu
- Die Mindeststunden können eingestellt werden, dies ist im Personalfenster möglich. Dort findet sich ein neuer Knopf mit der Bezeichnung Mindeststuden.
- Anzeige der Summe der Einsatzzeiten in der Personalübersicht und in der exportierten Tabelle. Diese Funktion ist immer aktiv und zeigt die Summe der Zeiten der jeweiligen Spalte an.
- Beim Export von Daten wird jetzt auch die Uhrzeit mit ausgegeben.

### Verbessert
- Kennzeichnung von externen Personen auf Triebfahrzeugen oder ähnlichem wurde vereinfacht und verbessert. Externe Personen können jetzt mit folgenden Begriffen gekennzeichnet werden, sodass sie als extern angesehen werden und nicht im Personalverzeichnis gesucht werden:
 - „Extern“
 - „Führerstand“
 - „FS“
 - „Schnupperkurs“
 - „ELF“
 - „Ehrenlokführer“
 - „ELF-Kurs“

 Hierbei ist es ausreichend, wenn einer dieser Begriffe in der Bemerkung vorkommt.
- Für registriertes Personal wurde eine ähnliche Funktion eingeführt, die verhindert, dass überprüft wird, ob eine Person die Qualifikation hat. Dies kann dann insbesondere für Ausbildungszwecke verwendet werden. Hier lauten die Schlüsselbegriffe:
 - „Azubi“
 - „Ausbildung“
 - „Tf-Ausbildung“,
 - „Zf-Ausbildung“
 - „Tf-Unterricht“
 - „Zf-Unterricht“
 - „Weiterbildung“
- In der Personalübersicht eines Arbeitseinsatzes und Fahrtages können jetzt die Aufgaben aus der voreingestellten Liste ausgewählt werden. Ebenso wurde ein extra Feld für die Bemerkung eingeführt. Die Zeiten können jetzt besser eingestellt werden.
- Die Sitzplätze bei Reservierungen können jetzt besser eingegeben werden. Hier wurde ein Fehler behoben.

### Fehlerbehebungen
- Beim öffnen einer Datei, wird jetzt wieder zu dem Monat gesprungen, an dem die Datei gespeichert wurde.
- Rechtschreibfehler korrigiert
- Weitere kleine Fehlerbehebungen und Verbesserungen

---

## Version 1.1.0 - 2016-12-21
### Neu
- Für jede Person kann eine zusätzliche Stundenanzahl für jedes Konto angegeben werden.<br>
Diese Zeiten können in der Einzelansicht einer Person angegeben werden.

### Verbessert
- Es muss kein Haken mehr gesetzt werden, wenn die Reservierungen automatisch verteilt werden sollen.
- Beim Export von Reservierungen wird jetzt auch der Zu- und Ausstieg angegeben.

### Fehlerbehebungen
- Die Aufgabe einer Person bei einer bestimmten Aktivität wird jetzt korrekt gespeichert und geladen
- Weitere kleinere Fehlerbehebungen und Verbesserungen

---

## Version 1.0.4 - 2016-12-14
### Fehlerbehebungen
- Der Fehler, das das Programm beim Export der Personalübersicht bei manchen Benutzern abstürzt, wurde behoben.
- Weitere kleine Fehlerkorrekturen und Verbesserungen.

---

## Version 1.0.3 - 2016-12-02
### Neu
- Beim Export werden jetzt die Aufgaben der Personen angegeben, wenn sie unter sonstigem Personal gelistet wurden
- Arbeitseinsätze werden jetzt standardmäßig in der Listenansicht exportiert
- Bei Nikolausfahrten werden die Reservierungen nicht mehr auf dem Übersichtsblatt ausgegeben, sie müssen ab sofort über die Funktionen Reservierungen exportieren ausgegeben werden

### Fehlerbehebungen
- Personen können jetzt wieder in die Personallisten korrekt eingetragen und gelöscht werden, auch wenn eine Bemerkung angegeben ist
- Die Auswahl eines Datum bei der Export-Funktion für das "Bis Datum" funktioniert jetzt wieder wie erwartet
- Weitere kleinere Verbesserungen und Fehlerbehebungen

---

## Version 1.0.2 - 2016-11-13
### Neu
- Wenn die Maus über dem Eintrag in der Seitenleiste verweilt, wird der Anlass der Veranstaltung angezeigt.
- Wichtige Fahrtage werden als solche in der Seitenleiste angezeigt.

### Verbessert
- Die Wagenreihung kann jetzt auch mit Leerräumen eingegeben werden.
- Personen können jetzt auch mit "Nachname, Vorname" in die Listen eingegeben werden.
- Die Einträge werden jetzt in der Seitenleiste richtig sortiert. Auch werden die Daten in der richtigen Reihenfolge exportiert.
- Die Personalübersicht wurde flexibeler gestaltet, auch können verschiedene Spalten angezeigt und exportiert werden.
- Der Text wird jetzt bei Bedarf im Kalender umgebrochen.
- Reservierungen werden jetzt auf Plausibilität geprüft (z.B. kein Einstig am letzten Haltepunkt eines Zuges).

### Fehlerbehebungen
- Allgemeine Verbesserungen bei der Stabilität und Behebung von Fehlern.

---

## Version 1.0.1 - 2016-10-07
### Fehlerbehebungen
- Problem beim Öffnen von Dateien behoben (Externe Personen wurden nicht geladen, Personen wurden nicht im Arbeitseinsatzfenster angezeigt)
- Problem bei Anzeige von Personen behoben (Die Informationen zu den Aktivitäten wurden unter Umständen falsch dargestellt)

---

## Version 1.0.0 - 2016-10-06
### Neu
#### Allgemein
- Verwaltung von Arbeitseinsätzen und Fahrtagen
- Neues Aussehen für das Startfenster: Ein Kalender ermöglicht die schnelle und sichere Navigation zu den Aktivitäten
- Verwaltung von Personal mit dessen Ausbildung
- Automatische Unterscheidung zwischen Zub. und Beil.o.b.A. aufgrund der Ausbildung der Personen
- Die Tabellen sind kompakter gestaltet und somit ist mehr Platz für Inhalt
- Unter macOS: Unterstützung der "öffnen mit" Funktion
- Automatisches Suchen nach Updates im Internet
- Speichert die letzte Position des Hauptfensters
- Neues Icon und Logo, das auch auf schwarzem und weißem Hintergrund gut zu erkennen ist
- Behebung von Fehlern und Problemen

#### Fahrtag
- Neue Export-Funktion: Hier werden jetzt nur Reservierungen ausgegeben, nach Wagen und Name sortiert.
- Es ist möglich zusätzliches personal einzutragen, sodass die Personen nicht in die vier Gruppen eingeteilt werden müssen
- Automatische Verteilung der Sitzplätze bei einer Nikolausfahrt
- Anzeige, zu wieviel Prozent die jeweiligen Klassen und der gesamte Zug belegt sind

#### Personal
- Anzeige, welche Person bei welchen Aktivitäten mitgeholfen hat
- Bestimmen der Einsatzzeiten des Personals für einzelne Arbeitsbereiche (Z.B. Werkstatt, Zugbegleitung)
