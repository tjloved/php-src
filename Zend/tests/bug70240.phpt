--TEST--
Bug #70240 (Segfault when doing unset($var()))
--FILE--
<?php

function fn836733554()
{
    unset($var());
}
fn836733554();
--EXPECTF--
Fatal error: Can't use function return value in write context in %sbug70240.php on line %d
