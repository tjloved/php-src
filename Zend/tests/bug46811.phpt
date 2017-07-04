--TEST--
ini_set() function
--INI--
arg_separator.output=&
--FILE--
<?php

function fn11050062()
{
    var_dump(ini_set("arg_separator.output", ""));
    var_dump(ini_get("arg_separator.output"));
}
fn11050062();
--EXPECTF--
bool(false)
string(1) "&"

