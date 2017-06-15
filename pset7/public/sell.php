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
            render("../views/sell_form.php",["title" => "Sell Stocks"]);
        }
        // otherwise, load the info for their requested stock symbol
        else{
            // check that symbol is valid
            if($stock = lookup($_POST["symbol"])){
                
                // check that they have the stock
                $rows = CS50::QUERY("SELECT * FROM portfolios WHERE symbol = ? AND user_id = ?", $_POST["symbol"], $_SESSION["id"]);
                
                if(empty($rows))
                    apologize("Sorry, you don't own any shares!");
                else{
                    
                    foreach($rows as $row)
                        $user_stock = $row;
                        
                    // check that they have enough shares to sell
                    if($_POST["shares"] > $user_stock["shares"]){
                        apologize("Sorry, you don't own enough shares!");
                    }
                    else{
                        
                        // subtract sales from owned shares
                        if($_POST["shares"] == $user_stock["shares"])
                            CS50::QUERY("DELETE FROM portfolios WHERE user_id = ? AND symbol = ?", $_SESSION["id"], $_POST["symbol"]);
                        
                        else
                            CS50::QUERY("UPDATE portfolios SET shares = shares - ? WHERE symbol = ? AND user_id = ?", $_POST["shares"], $stock["symbol"], $_SESSION["id"]);
                            
                        CS50::QUERY("UPDATE users SET cash = cash + ? WHERE id = ?", $_POST["shares"]*$stock["price"], $_SESSION["id"]);
                        CS50::QUERY("INSERT INTO history (user_id, symbol, trans_type, curr_price, shares) VALUES(?,?,?,?,?)", $_SESSION["id"], $_POST["symbol"], "SELL", $stock["price"], $_POST["shares"]);
                        redirect("index.php");
                        
                    }
                }
            }
            else{
                apologize("Stock symbol was invalid!");
            }
        }
        
    }

?>