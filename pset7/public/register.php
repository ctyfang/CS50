<?php

    // configuration
    require("../includes/config.php");

    // if user reached page via GET (as by clicking a link or via redirect)
    if ($_SERVER["REQUEST_METHOD"] == "GET")
    {
        // else render form
        render("register_form.php", ["title" => "Register"]);
    }

    // else if user reached page via POST (as by submitting a form via POST)
    else if ($_SERVER["REQUEST_METHOD"] == "POST")
    {
        // check for empty fields
        
        // span is used to style in-line elements
        // fieldset is used to group together related fields - draws a box
        
        if((empty($_POST["username"])) || (empty($_POST["password"])) || empty(($_POST["confirmation"])) || ($_POST["password"] != $_POST["confirmation"])){
            
            if(empty($_POST["username"])){
                $err_message = "Please enter a username!";
            }
            elseif(empty($_POST["password"])){
                $err_message = "Please enter a password!";    
            }
            elseif(empty($_POST["confirmation"])){
                $err_message = "Please confirm your password!";   
            }
            else{
                $err_message = "Passwords do not match!";
            }
                
            apologize($err_message);
            
        }
            
            
            
        else{
            //query sql, add user to database, hash their pass via password_hash()
            
            if(CS50::QUERY("INSERT IGNORE INTO users (username, hash, cash) VALUES (?,?,10000.0000)",$_POST["username"], password_hash($_POST["password"], PASSWORD_DEFAULT))){
                //store their ID via SELECT LAST_INSERT_ID() AS id
                $rows = CS50::QUERY("SELECT LAST_INSERT_ID() AS id");
                $id = $rows[0]["id"];
                
                //log them in by setting that id in session array
                $_SESSION["id"] = $id;
                
                redirect("../public/index.php");
                
            }
            else{
                apologize("Username is already taken");
            }
                
        }    
        
        
    }

?>