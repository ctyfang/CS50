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
        if(empty($_POST["symbol"]) || empty($_POST["shares"])){
            //render view for stock request if no request has been made or if error in request
            render("../views/buy_form.php",["title" => "Sell Stocks"]);
        }
        // otherwise, load the info for their requested stock symbol
        else{
            // check that symbol is valid
            if($stock = lookup($_POST["symbol"])){
                
                $rows = CS50::QUERY("SELECT * FROM users WHERE id = ?", $_SESSION["id"]);
                
                foreach($rows as $row)
                    $user_info = $row;
                    
                // check that they have enough money
                if($user_info["cash"] < ($stock["price"]*$_POST["shares"]))
                    apologize("You don't have enough cash!");
                else{
                    
                    // if they have enough money, check if they already own it
                    $rows = CS50::QUERY("SELECT * FROM portfolios WHERE symbol = ? AND user_id =?", $_POST["symbol"], $_SESSION["id"]);
                    
                    foreach($rows as $row)
                        $curr_portfolio = $row;
                        
                    // if they don't own it
                    if(empty($curr_portfolio))
                        CS50::QUERY("INSERT INTO portfolios (symbol, user_id, shares) VALUES(?,?,?)", $_POST["symbol"], $_SESSION["id"], $_POST["shares"]);
                        
                        // subtract cash from stores and redirect to index
                    else
                        CS50::QUERY("UPDATE portfolios SET shares = shares + ? WHERE symbol = ? AND user_id = ?", $_POST["shares"], $stock["symbol"], $_SESSION["id"]);
                    
                    
                    CS50::QUERY("UPDATE users SET cash = cash - ? WHERE id = ?", $_POST["shares"]*$stock["price"], $_SESSION["id"]);
                    CS50::QUERY("INSERT INTO history (user_id, symbol, trans_type, curr_price, shares) VALUES(?,?,?,?,?)", $_SESSION["id"], $_POST["symbol"], "BUY", $stock["price"], $_POST["shares"]);
                        
                    redirect("index.php");
             
                    
                    // update cash reservers and redirect
                        
                }
                
            }
            else{
                apologize("Stock symbol was invalid!");
            }
        }
        
    }

?>