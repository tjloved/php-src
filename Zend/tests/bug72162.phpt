--TEST--
Bug #72162 (use-after-free - error_reporting)
--FILE--
<?php

function fn1003890250()
{
    error_reporting(E_ALL);
    $var11 = new StdClass();
    $var16 = error_reporting($var11);
}
fn1003890250();
--EXPECTF--
Recoverable fatal error: Object of class stdClass could not be converted to string in %sbug72162.php on line %d
