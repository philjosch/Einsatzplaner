<?php
    $validId = "samson";
    if (isset($_GET["id"])) {
        if ($_GET["id"] == $validId) {
            die("OK");
        } else {
            die("error");
        }
    }
    
    
    $target = "files/";
    $target = $target . basename( $_FILES['uploaded']['name']) ;
//    echo $target;
    $ok=1;
    
//    print_r($_FILES);
//    print_r($_POST);
//    print_r($_GET);
    
    //This is our size condition
    if ($uploaded_size > 10000000)
    {
        echo "Your file is too large.<br>";
        $ok=0;
    }
    
    //This is our limit file type condition
    if ($uploaded_type =="text/php")
    {
        echo "No PHP files<br>";
        $ok=0;
    }
    
    //Here we check that $ok was not set to 0 by an error
    if ($ok==0)
    {
        echo "Sorry your file was not uploaded";
    }
    
    //If everything is ok we try to upload it
    else
    {
        if(move_uploaded_file($_FILES['uploaded']['tmp_name'], $target))
        {
            die("OK");
            echo "The file successfully ". basename( $_FILES['uploadedfile']['name']). " has been uploaded";
        }
        else
        {
            echo "Sorry, there was a problem uploading your file.";
        }
    }
    ?>
