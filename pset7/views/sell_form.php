<form action="sell.php" method="post">
    <fieldset>
        <div class="form-group">
            <input autocomplete="off" autofocus class="form-control" name="symbol" placeholder="Stock Symbol" type="text"/>
        </div>
        <div class="form-group">
            <input class="form-control" name="shares" placeholder="Number of Shares" type="text/>
        </div>
        <div class="form-group">
            <button class="btn btn-default" type="submit">
                Request Sale
            </button>
        </div>
    </fieldset>
</form>
<div>
    or <a href="quote.php">Check current prices!</a>
</div>
