#!/usr/bin/env php
<?php
    
    // TODO: iterate over US.txt and import each row of data into mysql
    // require CS50.php for query function
    require("../includes/config.php");
    
    // check that file exists and is readable
    if(!is_readable($argv[1])){
        echo "File doesn't exist or isn't readable!";
    }
    
    // open the file for reading, $handle is a file pointer
    if($handle = fopen($argv[1], "r")){
        while($row = fgetcsv($handle, 0, "\t")){
            
            $country = $row[0];
            $postal = $row[1];
            $place = $row[2];
            $admin_name1 = $row[3];
            $admin_code1 = $row[4];
            $admin_name2 = $row[5];
            $admin_code2 = $row[6];
            $admin_name3 = $row[7];
            $admin_code3 = $row[8];
            $lat = $row[9];
            $lng = $row[10];
            $acc = $row[11];
            
            CS50::QUERY("INSERT INTO places 
            (country_code, postal_code, place_name, admin_name1, admin_code1, admin_name2, admin_code2, admin_name3, admin_code3, latitude, longitude, accuracy) 
             VALUES (?,?,?,?,?,?,?,?,?,?,?,?)", $country, $postal, $place, $admin_name1, $admin_code1, $admin_name2, $admin_code2, $admin_name3, $admin_code3,
             $lat, $lng, $acc);
             
        }
        
    }
    else{
        echo "File couldn't be opened!";
    }
    
    echo "Import success!";
    
?>