<?php
    /* -------------------------------------------------
     *
     * All Rights by Philipp Schepper 2016-2019
     *
     * -------------------------------------------------
     */


    // Die Datei mit der Konfiguration des Servers einbinden.
    require './config.php';

    // Pruefen, ob Testfunktion aufgerufen wurde
    if (isset($_GET["id"])) {
        if (in_array($_GET['id'], $validIDs)) {
            die("OK");
        } else {
            die("ID");
        }
    }

    // Pruefen, ob ueberhaupt Dateien hochgeladen wurden.
    if (! isset($_FILES['uploaded'])) { die("FILE"); }

    // Pruefen, ob die ID vorhanden ist
    if (! isset($_POST['id'])) { die("ID"); }
    if (! in_array($_POST['id'], $validIDs)) { die("ID"); }
    
    // Datei Variablen auslesen
    $name = $_FILES['uploaded']['name'];
    $type = $_FILES['uploaded']['type'];
    $tmp_name = $_FILES['uploaded']['tmp_name'];
    $size = $_FILES['uploaded']['size'];

    // Abpruefen, ob der Dateiname geaendert werden soll
    if (! empty($fileNames[array_search($_POST['id'], $validIDs)])) {
        $name = $fileNames[array_search($_POST['id'], $validIDs)];
    }

    // Groesse der Datei beschraenken
    if ($uploaded_size > 10000000) { die("SIZE"); }
    // Nur PDF-Dateien erlauben
    if ($type != "application/pdf") { die("TYPE"); }

    // Pfad erstellen
    $target = BASE_DIRECTORY . basename($name) ;
    // Datei hochladen
    if(move_uploaded_file($tmp_name, $target)) {
        die("OK");
    } else {
        die("ERROR");
    }
    ?>
