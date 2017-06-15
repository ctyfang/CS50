#!/usr/bin/env php
<?php
    
    $search_str = "12038";
    
    preg_match_all('/(\d+(\.\d+)?)/', $search_str, $out);
    $parsed = $out[0];
    
    if(!is_numeric($parsed[1]))
        echo "GPS entered";
    else
        echo "Postal code entered";
        
    
?>