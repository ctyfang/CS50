<?php

    // configuration
    require("../includes/config.php");

    // if the user isn't logged in, render log-in page
    if (empty($_SESSION["id"]))
    {
        // else render form
        render("login_form.php", ["title" => "Log-in"]);
    }
    // otherwise, if they are logged in
    else
    {
        // if they haven't entered a stock symbol
        if(empty($_POST["symbol"])){
            //render view for stock request
            render("../views/quote_req.php",["title" => "Quote Request"]);
        }
        // otherwise, load the info for their requested stock symbol
        else{
            //render info page for requested stock
            if($stock = lookup($_POST["symbol"]))
                render("../views/quote_resp.php", $stock);
            else{
                apologize("Stock symbol was invalid!");
            }
        }
        
    }

?>