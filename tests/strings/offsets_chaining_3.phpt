--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php

function fn1748223797()
{
    $string = "foobar";
    var_dump(isset($string[0][0][0][0]));
}
fn1748223797();
--EXPECTF--
bool(true)

