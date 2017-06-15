<?php

    // configuration
    require("../includes/config.php"); 

    // store users table info 
    $rows = CS50::QUERY("SELECT * FROM users WHERE id = ?", $_SESSION["id"]);
    
    foreach($rows as $info){
        $user_info = $info;
    }
    
    // store stock info as positions
    $stocks_owned = CS50::QUERY("SELECT * FROM portfolios WHERE user_id = ?", $_SESSION["id"]);
            
    foreach ($stocks_owned as $row)
    {
        $stock = lookup($row["symbol"]);
        if ($stock !== false)
        {
            $positions[] = [
            "name" => $stock["name"],
            "price" => $stock["price"],
            "shares" => $row["shares"],
            "symbol" => $row["symbol"]
            ];        
        }
    }
                    
    
    // render portfolio
    render("portfolio.php", ["positions" => $positions, "title" => "Portfolio", "userinfo" => $user_info]);

?>
