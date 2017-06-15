<?php
    $max = $_GET["num_spec"];
?>

<!DOCTYPE HTML>

<html>
    <header>
        <style type="text/css">
            .test{
                border-style:solid;
                border-width:2px;
                border-color:black;
                color:white;
                text-align:center;
                padding: 5px;
            }
            
            .test2{
                background-color:green;
            }
            
        </style>
    </header>
    
    <body>
        <table class='test2'>
            <?php
                for($j = 1; $j <= $max; $j++){
                    printf('<tr>');
                    for($i = 1; $i <= $max; $i++){
                        printf('<td class = \'test\'>');
                        printf("%5d", $i*$j);
                        printf('</td>');
                        
                    } 
                    printf('</tr>');
                }
            ?>
        </table>
        
    </body>
    
</html>