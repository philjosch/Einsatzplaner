<?php
    /* ALLGEMEINE KONFIGURATIONSDATEI */
    
    /* Pfad zu dem Ordner, in dem die Hochgeladenen Dateien gesichert werden
     * Muss ohne '/' starten und mit '/' enden!
     */
    define("BASE_DIRECTORY",    "files/");
    
    /* Liste mit den zugelassenen IDs.
     * Die IDs dienen dazu den Uploadprozess zu legitimieren.
     * Um die Eindeutigkeit zu erhoehen sollten sie mindestens 8 Zeichen lang sein,
     * und aus Gross- und Kleinbuchstaben und Zahlen bestehen.
     *
     * Beispiel:
     * $validIDs = array("test", "zweiterTest", "Test3.0");
     */
    $validIDs = array("test");
?>
