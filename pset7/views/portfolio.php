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
        <th>SYMBOL</th>
        <th>NAME</th>
        <th>SHARES</th>
        <th>TOTAL</th>
        
        <tr>
            <td>CASH</td>
            <td></td>
            <td></td>
            <td>
                <?= "$" . $userinfo["cash"] ?>
            </td>
        </tr>
        
        <?php foreach ($positions as $position): ?>

            <tr>
                <td><?= $position["symbol"] ?></td>
                <td><?= $position["name"] ?></td>
                <td><?= $position["shares"] ?></td>
                <td><?= "$" . $position["price"]*$position["shares"] ?></td>
            </tr>
        
        <?php endforeach ?>
        
        
        
    </table>
</div>


