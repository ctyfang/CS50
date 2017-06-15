<?php

    // configuration
    require("../includes/config.php"); 

    // store users table info 
    $rows = CS50::QUERY("SELECT * FROM history WHERE user_id = ?", $_SESSION["id"]);
    
    $positions = [];
    
    foreach ($rows as $row)
    {
        
        $positions[] = [
            "time" => $row["time"],
            "trans_type" => $row["trans_type"],
            "symbol" => $row["symbol"],
            "shares" => $row["shares"],
            "price" => $row["curr_price"]
    
        ];        
        
    }
                    
    
    // render portfolio
    render("history_table.php", ["positions" => $positions, "title" => "History"]);

?>
