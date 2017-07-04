--TEST--
testing the behavior of string offset chaining
--INI--
error_reporting=E_ALL | E_DEPRECATED
--FILE--
<?php

function fn370230858()
{
    $string = "foobar";
    var_dump($string[0][0][0][0]);
}
fn370230858();
--EXPECTF--
string(1) "f"

