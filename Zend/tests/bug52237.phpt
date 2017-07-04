--TEST--
Bug #52237 (Crash when passing the reference of the property of a non-object)
--FILE--
<?php

function fn1193666772()
{
    $data = 'test';
    preg_match('//', '', $data->info);
    var_dump($data);
}
fn1193666772();
--EXPECTF--

Warning: Attempt to modify property of non-object in %sbug52237.php on line %d
string(4) "test"
