# Einsatzplaner - Veränderungshistorie
Stand: 24. September 2018

## Version 1.4.1 – 2018-09-??
### Neu
- Kalender zeigt jetzt den Anlass einer Aktivität und eines Fahrtages an, sofern es möglich ist

### Verbessert
- Fahrtagfenster und Fenster für Aktivitäten optimiert, sodass sie den Platz besser nutzen
- Verbesserter Export von Arbeitseinsätzen, sowohl in der Listen- als auch in der Einzelansich
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
