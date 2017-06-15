<style>
    
    table{
        table-layout: fixed;
    }
    
    th{
        border-bottom: 1px solid #ddd;
    }
    
    th, td {
        text-align: center;
        height: 10px;
        width: 400px;
        
    }
    
 

    
</style>

<div>
    <br>
    <table>
        <th>TIME</th>
        <th>TRANSACTION</th>
        <th>SYMBOL</th>
        <th>SHARES</th>
        <th>UNIT PRICE</th>
        <th>NET CASH</th>
        
        <?php foreach ($positions as $position): ?>

            <tr>
                <td><?= $position["time"] ?></td>
                <td><?= $position["trans_type"] ?></td>
                <td><?= $position["symbol"] ?></td>
                <td><?= $position["shares"] ?></td>
                <td><?= $position["price"] ?></td>
                <td><?php
                        if($position["trans_type"] == "BUY")
                            echo "-";
                        else
                            echo "+";
                            
                        echo "$" . $position["price"]*$position["shares"] ;
                        
                    ?></td>
            </tr>
        
        <?php endforeach ?>
        
        
        
    </table>
</div>


