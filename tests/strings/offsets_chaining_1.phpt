--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php

function fn2123699128()
{
    $string = "foobar";
    var_dump($string[0][0][0][0]);
}
fn2123699128();
--EXPECTF--
string(1) "f"

