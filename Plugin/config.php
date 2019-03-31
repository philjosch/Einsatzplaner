<?php
    /* ALLGEMEINE KONFIGURATIONSDATEI */
    
    /* Pfad zu dem Ordner, in dem die Hochgeladenen Dateien gesichert werden
     * Muss ohne '/' starten und mit '/' enden!
     */
    define("BASE_DIRECTORY",    "files/");
    
    /* Liste mit den zugelassenen IDs und den zugehoerigen Dateinamen
     *
     * Die IDs dienen dazu den Uploadprozess zu legitimieren.
     * Um die Eindeutigkeit zu erhoehen sollten sie mindestens 8 Zeichen lang sein,
     * und aus Gross- und Kleinbuchstaben und Zahlen bestehen.
     *
     * Die Liste $fileNames enthaelt die Namen der dateien,
     * unter denen die Datei gespeichert werden soll.
     * Fuer jede ID muss es einen Wert in $fileNames geben
     * Wenn der Wert "" ist, wird der Dateiname verwendet,
     * der in den Daten spezifiziert wurde.
     *
     * Beispiel:
     * $validIDs  = array("test", "zweiterTest", "Test3.0");
     * $fileNames = array("",     "Plan.pdf",    "Planung.pdf");
     */
    $validIDs  = array("test");
    $fileNames = array("");
?>
