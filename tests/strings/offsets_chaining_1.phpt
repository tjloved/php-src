--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php

function fn839100357()
{
    $string = "foobar";
    var_dump($string[0][0][0][0]);
}
fn839100357();
--EXPECTF--
string(1) "f"

