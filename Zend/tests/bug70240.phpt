--TEST--
Bug #70240 (Segfault when doing unset($var()))
--FILE--
<?php

function fn1114958653()
{
    unset($var());
}
fn1114958653();
--EXPECTF--
Fatal error: Can't use function return value in write context in %sbug70240.php on line %d
