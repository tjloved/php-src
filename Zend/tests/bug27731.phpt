--TEST--
Bug #27731 (error_reporing() call inside @ block does not work correctly)
--FILE--
<?php

function fn106895293()
{
    error_reporting(E_ALL ^ E_NOTICE);
    @error_reporting(E_WARNING);
    var_dump(error_reporting());
}
fn106895293();
--EXPECT--
int(2)
