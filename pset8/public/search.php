<?php

    require(__DIR__ . "/../includes/config.php");

    // numerically indexed array of places
    $places = [];

    // TODO: search database for places matching $_GET["geo"], store in $places for autocomplete
    // Most likely an incomplete string for  a city, state, or postal code
    // Consider use of SQL LIKE and MATCH operators
    
    // IDEAS:
    // -Find matches for string in various columns and return whatever is returned (leaves heavylifting to SQL)
    
    $search_str = $_GET["geo"];
    
    // check if search_str has floats or ints
    if(is_numeric($search_str)){
        // employs regex pattern match to group substrings
        preg_match_all('/(\d+(\.\d+)?)/', $search_str, $out);
        
        $parsed = $out[0];
        
        if(is_numeric($parsed[1]))
            $rows = CS50::QUERY("SELECT * FROM places WHERE latitude LIKE ? AND longitude LIKE ?", $parsed[0] . "%", $parsed[1] . "%");

        else
            $rows = CS50::QUERY("SELECT * FROM places WHERE postal_code LIKE ?", $search_str."%");
    }
    else
        $rows = CS50::QUERY("SELECT * FROM places WHERE MATCH(place_name, admin_name1, admin_code1) AGAINST (?)", $search_str);   
    
    foreach($rows as $row){
        $places[] = $row;
    }
    // output places as JSON (pretty-printed for debugging convenience)
    header("Content-type: application/json");
    print(json_encode($places, JSON_PRETTY_PRINT));

?>