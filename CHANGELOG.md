# Einsatzplaner - Veränderungshistorie
Stand: 9. August 2021

## Version 1.7.2
### Neu
- Als Kontoinhaber wird standardmäßig der Name der Person gewählt 
- Dienstzeiten bei Fahrtagen können minutengenau angegeben werden
- Heutiger Tag wird im Kalender markiert
- Zugriff auf alle Personaldaten im Einsatzplaner

### Verbessert
- Export von Daten verbessert (Zeiten enthalten immer Minuten)
- Diverse Fenster (Person, Fahrtag, Personal, Einstellungen, Dateieigenschaften...) verbessert
- Der automatische Upload funktioniert nur über eine verschlüsselte Verbindung
- Warnung bei Upload-Fehler
- Dunkelmodus für Exportdialog
- Diverse Verbesserungen und Fehlerbehebungen

### Fehlerbehebungen
- Beim Export der Einsatzzeiten wurden die Anzahl und die KIlometer nicht ausgegeben
- Der Export von Aktivitäten war nicht über das Menü im Kalender möglich
- Bei der Exportfunktion wurden auch vergangene Aktivitäten ausgegeben, obwohl "ab jetzt" gewählt war
- Überschrift in der erstellten CSV-Datei war teilweise falsch
- Export von Reservierungen eines Fahrtages war fehlerhaft 


## Version 1.7.1 – 2021-06-13
### Verbessert
- Die End-Zeit für Arbeitseinsätze wird dynamisch bestimmt (18Uhr im Sommer, 16 Uhr im Winter)
- Mehr Begriffe für die automatische Kategorieerkennung bei Arbeitseinsätzen
- Diverse interne Verbesserungen und Optimierungen
- Die Personaltabelle des Personalplaners wird nicht bei jeder Änderung aktualisiert
- Ausgabe bei Personalisten, wie viele Personen ausgegeben wurden
- Diverse kleinere Verbesserungen

### Fehlerbehebungen
- Die Exportfunktion gab keine Aktivitäten aus, wenn "Bis Ende des Jahres" oder "Ab Anfang des Jahres" ausgewählt wurde
- Tb-Stunden wurden nicht ausgegeben
- Büro-Tätigkeiten erscheinen nicht in der Listenansicht
- Tabelle für das Personal hatte unter Umständen keine Überschrift
- Gefahrene Strecke wurde falsch berechnet
- Kontoinhaber falsch dargestellt, wenn nicht `Familienbeitrag (Nutzer)` ausgewählt war
- Einträge in der Personaltabelle eines Fahrtags konnte unter Umständen unberechtigt geändert werden
- Darstellung von Farben im Dunkelmodus wurden verbessert
- Diverse kleinere Fehlerbehebungen


## Version 1.7.0 – 2021-04-02
### Neu
- Programm in Einsatzplaner und Personalplaner aufgeteilt und neue Icons eingeführt
- Absagen von Fahrtagen und Arbeitseinsätzen
- Verschiedene weitere Personaldaten eingeführt (u.a. Persönliche Daten, Mitgliedschaft)
- Eine Person kann mehrfach in eine Aktivität eingetragen werden, auch bei gleicher Kategorie
- Datei kann verschlüsselt und mit Passwortschutz gespeichert werden

### Verbessert
- Auch eine leere Liste mit Aktivitäten kann ausgegeben werden
- Toolbar eingeführt, um auf wichtige Funktionen zuzugreifen
- Arbeitseinsätze können auch als ``wichtig`` markiert werden
- Export der Mitgliederdaten kann individualisiert werden (nur ausgewählte Daten werden ausgegeben)
- Gleiche Einfärbung von Aktivitäten im Programm und für den Export
- Datei wird komprimiert gespeichert, um Speicherplatz zu sparen
- Diverse interne Verbesserungen und Optimierungen

### Fehlerbehebungen
- Das Programm konnte abstürzen, wenn eine Aktivität gelöscht wurde
- Die Datei mit den Stammdaten konnte nicht geöffnet werden
- Die hochgeladene Datei war unter Umständen leer bzw. fehlerhaft
- Diverse kleinere Fehlerbehebungen

---

## Version 1.6.2 – 2020-10-16
### Neu
- Die Mindeststunden werden nicht mehr für Mitglieder berechnet, die noch nicht volljährig sind.
- Schreibschutz für geöffnete Dateien eingefügt.

### Verbessert
- Vergangene Aktivitäten des gleichen Tages werden nicht mehr ausgegeben, wenn ``ab Heute`` bzw. ``ab Jetzt`` gewählt wird.
- Dialog der Exportfunktion überarbeitet
- Export der Daten verbessert: Gesonderte Hervorhebung, wenn Personal bei Aktivitäten benötigt wird, die in Kürze (10 Tage) anstehen.
- Die Standardeinstellungen für den Export auf Druckern wird auch beim Datei-Upload benutzt.

### Fehlerbehebungen
- Behebt mehrere Fehler durch die das Programm abstürzt, wenn eine Reservierung, ein Fahrtag oder ein Arbeitseinsatz gelöscht wird.
- Zeilenumbrüche werden wieder korrekt ausgegeben und angezeigt.
- Diverse Verbesserungen und Optimierungen


## Version 1.6.1 – 2020-08-06
### Neu
- Beim Export der Mitglieder werden nur noch die Mitglieder ausgegeben, die aktuell auch angezeigt werden.
- Die Personaldaten werden im Personalfenster in einer Übersicht angezeigt.
- Eine Statistik zeigt unter anderem an, wie viele Mitglieder aktiv oder passiv sind.
- Aktive werden nur noch anhand der geleisteten Gesamtstunden bewertet. Die Markierung der nicht erbrachten Mindeststunden für andere Kategorien bleibt aber bestehen.
- Die Sortierung der Namen in der Personalübersicht kann zwischen ``Vorname Nachname``und ``Nachname, Vorname`` eingestellt werden.
- Die Personaldaten können auch als Detailansicht ausgegeben werden, ohne die Liste für alle Mitglieder ausgeben zu müssen.


### Verbessert
- Beim Löschen von Personen wurde eine Sicherheitsabfrage eingefügt, um versehentliches Löschen von Personen zu vermeiden.
- Mail-Adressen, die mehreren Mitgliedern zugeordnet sind, führen nicht mehr dazu, dass evtl. mehrere Mails an diese Adresse gesendet werden.
- Bei der Eingabe von Namen werden führende und abschließende Leerzeichen ignoriert.
- Per Knopfdruck kann jetzt auch eine Mail an eine einzelne Person geschrieben werden.

### Fehlerbehebungen
- Die hochgeladene Datei wird wieder im Querformat dargestellt.
- Personal, dass der Tätigkeit ``Infrastruktur`` zugeordnet ist, wird wieder in der Listenansicht ausgegeben.
- Behebt einen Fehler, bei dem das Programm abstürzt, wenn bestimmte Personen von Aktivitäten gelöscht werden.
- Behebt einen Fehler, durch den die eingegebenen Verbindungsdaten zum EPL-Server nicht überprüft wurden.


## Version 1.6 – 2020-05-02
### Neu
- Die Personalverwaltung wurde verbessert, indem jetzt alle Vereinsmitglieder aufgenommen und verwaltet werden können. Ebenso können verschiedene persönliche Daten und Kontaktdaten eingegeben und auch entsprechend exportiert weden.
- Eine Person kann jetzt mehrfach bei einem Arbeitseinsatz oder Fahrtag eingetragen werden, vorausgesetzt die Aufgabe ist jeweils verschieden.
- Anzeige der Auslastung der einzelnen Züge anhand der eingetragenen Reservierungen.
- Die Liste der Reservierungen kann nach Zügen gefiltert werden.
- Komplett überarbeitete Dokumentation.

### Verbessert
- Es gibt jetzt eine neue Kategorie "Infrastruktur". Diese kann z.B. für Streckenarbeiten und Vegetationskontrollen genutzt werden.
- Die Anzahl der benötigten Lokführer kann beliebig zwischen null und zwei festgelegt werden, falls z.B. mit mehr als einem Triebfahrzeug gefahren wird.
- Die zusätzlichen Stunden und Mindeststunden können jetzt minutengenau eingegeben werden.
- Die Summe der Spalten in der Tabelle der Gesamtübersicht bezieht sich immer auf die aktuell angezeigten Personen.
- Beim Export von Daten können im Druckerdialog jetzt auch Seitenformat und Ausrichtung bestimmt werden.
- Unzählige Verbesserungen und Optimierungen "unter der Haube", um die Geschwindigkeit und den Speicherverbrauch zu optimieren.

### Fehlerbehebungen
- Fehler bei der Akzeptanz bestimmter Fahrstrecken einer Reservierung behoben.
- Die Einträge der Aktivitäten im Kalender bleiben nicht mehr markiert.
- Verschiedene kleinere Fehler behoben.

---

## Version 1.5.2 – 2020-03-22
### Neu
- Durch einen Doppelklick auf eine Person in der Gesamtübersicht des Personalfensters wird die entsprechende Einzelansicht angezeigt.

### Verbessert
- Export der Personalübersichten verbessert, indem Stunden besser formatiert werden.

### Fehlerbehebungen
- Ein Fehler wurde behoben, bei dem das Programm beim Beenden abstürzt, wenn das automatische Speichern deaktiviert war.
- Beim Export der Personaldaten werden die Dateieinstellungen übernommen.


## Version 1.5.1 – 2019-11-27
### Neu
- Die Einsatzzeiten einer einzelnen Person können ab sofort als PDF gespeichert und gedruckt werden.

### Verbessert
- Der Export der Aktivitäten als Listen- und Einzelansicht wurde optimiert.
- Der Export der Personaldaten als Listen- und Einzelansicht wurde verbessert.

### Fehlerbehebungen
- Externes Personal eines Fahrtages wird jetzt wieder im entsprechenden Fenster dargestellt.
- Die Personalübersicht bleibt sortiert, auch wenn sie aktualisiert wird.
- Ein Problem beim automatischen Speichern wurde behoben.
- Die Einstellung des Zeitraums beim Datei-Upload wird jetzt zuverlässig verwendet.
- Kleinere Verbesserungen und Fehlerbehebungen


## Version 1.5 – 2019-03-31
### Neu
- Der Einsatzplan kann als PDF direkt aus dem Programm auf einen vorher konfigurierten Webserver hochgeladen werden. Dies geschieht entweder beim Speichern oder manuell.
- Automatisches Sichern: Das Programm speichert bei Wunsch nach einer bestimmten Zeit automatisch eine Backup-Datei.
- Noch unbekannte Einsatzzeiten können jetzt als solche ausgewiesen werden.
- Fahrtage und Arbeitseinsätze können direkt aus dem jeweiligen Fenster heraus gelöscht werden.

### Verbessert
- In der Personalübersicht werden jetzt auch die Mindeststunden der Personen angezeigt.
- In der Listenansicht von Arbeitseinsätzen wurden redundante Informationen entfernt.
- In der Listenansicht von Arbeitseinsätzen wird jetzt auch ein etwaiger Ort angegeben.
- Eine Aktivität wird erst nach einer Sicherheitsabfrage gelöscht.

### Fehlerbehebungen
- Beim Eintragen von Personal für einen Arbeitseinsatz wurde die Person unter Umständen nicht richtig übernommen.
- Kleinere Optimierungen und Verbesserungen

---

## Version 1.4.1 – 2018-09-25
### Neu
- Kalender zeigt jetzt den Anlass einer Aktivität und eines Fahrtages an, sofern es möglich ist

### Verbessert
- Fahrtagfenster und Fenster für Aktivitäten optimiert, sodass sie den Platz besser nutzen
- Verbesserter Export von Arbeitseinsätzen, sowohl in der Listen- als auch in der Einzelansicht
- Fahrtage und Aktivitäten werden jetzt nicht mehr nur nach Datum sondern auch nach Beginn und Endzeit sortiert
- Die Sitzplätze werden jetzt besser und schneller verteilt
- Das Programm benötigt bei längerem Betrieb weniger Arbeitsspeicher
- Code optimiert und kleinere Funktionen verbessert

### Fehlerbehebungen
- Begleiter ohne betriebliche Aufgaben erschienen unter Umständen nicht in der Einzelansicht
- Fahrtage und Aktivitäten können jetzt ohne Probleme gelöscht werden
- Bei einem Arbeitseinsatz wird jetzt die Kategorie auch nach einem erneuten Öffnen intelligent bestimmt
- Bei der Personalübersicht werden die Zeiten für die Ausbildung verlässlich angezeigt
- Weitere Fehlerbehebungen zur Verbesserung der Stabilität


## Version 1.4 - 2018-07-23
### Neu
- Das Programm merkt sich die zuletzt verwendeten Dateien. Sie können unter "Datei>Zuletzt benutzt" direkt geöffnet werden.
- Im Kalender können jetzt direkt Arbeitseinsätze an einem ausgewählten Tag erstellt werden.

### Verbessert
- Für eine Person kann jetzt auch noch eine zusätzliche Anzahl an Aktivitäten angegeben werden. Bisher war dies nur für Zeiten und Kilometer möglich.

### Fehlerbehebungen
- Die Druckausgabe wurde verbessert. Unter macOS ist nun das korrekte Erstellen einer PDF-Datei möglich.
- Mehrere Fehler beim Kalender wurden behoben.
- Weitere verschiedene Fehlerbehebungen und Verbesserungen, unter anderem bei der Verwaltung von Reservierungen.

---

## Version 1.3.1 - 2018-03-14
### Neu
- Das Arbeitseinsatzfenster wurde intelligenter. Es wählt eine bestimmte Kategorie für die Personaltabelle aus, wenn es dies aus dem Anlass folgern kann.

### Verbessert
- Wird Personal für einen Fahrtag oder Arbeitseinsatz benötigt, wird dies farblich in der Einzelansicht kenntlich gemacht.
- Das Personal, dass bei einem Schnupperkurs eingetragen wurde, wird jetzt in der Listen- und Einzelansicht ausgegeben
- Externe Personen können nun auch mit "Nachname, Vorname" in Listen eingetragen werden.

### Fehlerbehebungen
- Probleme beim Drucken der Einzel- und Listenansicht zumindest unter macOS, wenn im Druckdialog "als PDF speichern" oder ähnliches gewählt wurde, sind behoben.
- Veränderungen in der Personaltabelle einer Aktivität wurde nicht korrekt übernommen.
- Fehler behoben, der es ermögliche nicht-Betriebsdienstpersonal als Zub einzutragen.
- Weitere Fehlerbehebungen, u.A. beim Darstellen der Daten nach dem Öffnen der Datei.


## Version 1.3 - 2017-12-23
### Neu
- Es kann jetzt eine Übersicht über die einzelnen Aktivitäten einer person ausgegeben werden, die Funktio dazu findet sich in der Einzelansicht des Personalfensters.
- Als weitere Kategorie für eine Aufgabe, kann jetzt noch "Ausbildung" angegeben werden.
- Es können nun auch zusätzliche Kilometer eingegeben werden, diese werden auf die automatisch berechneten Kilometer angerechnet.

### Verbessert
- Bei einer Reservieurng kann jetzt eine zweite Teilstrecke eingegeben werden.

### Fehlerbehebungen
- Bei einer Änderung an einer Reservierung wird vor dem Schließen wieder nachgefragt, ob die Änderungen übernommen werden sollen.
- Optimierung beim Personalfenster, sodass es kleiner als bisher gemacht werden kann.

---

## Version 1.2 - 2017-10-10
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
- Beim Öffnen eines Fahrtags wird die Bemerkung wieder geladen.
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


## Version 1.0.3 - 2016-12-02
### Neu
- Beim Export werden jetzt die Aufgaben der Personen angegeben, wenn sie unter sonstigem Personal gelistet wurden
- Arbeitseinsätze werden jetzt standardmäßig in der Listenansicht exportiert
- Bei Nikolausfahrten werden die Reservierungen nicht mehr auf dem Übersichtsblatt ausgegeben, sie müssen ab sofort über die Funktionen Reservierungen exportieren ausgegeben werden

### Fehlerbehebungen
- Personen können jetzt wieder in die Personallisten korrekt eingetragen und gelöscht werden, auch wenn eine Bemerkung angegeben ist
- Die Auswahl eines Datum bei der Export-Funktion für das "Bis Datum" funktioniert jetzt wieder wie erwartet
- Weitere kleinere Verbesserungen und Fehlerbehebungen


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


## Version 1.0.1 - 2016-10-07
### Fehlerbehebungen
- Problem beim Öffnen von Dateien behoben (Externe Personen wurden nicht geladen, Personen wurden nicht im Arbeitseinsatzfenster angezeigt)
- Problem bei Anzeige von Personen behoben (Die Informationen zu den Aktivitäten wurden unter Umständen falsch dargestellt)


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
