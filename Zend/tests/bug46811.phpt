--TEST--
ini_set() function
--INI--
arg_separator.output=&
--FILE--
<?php

function fn1171036738()
{
    var_dump(ini_set("arg_separator.output", ""));
    var_dump(ini_get("arg_separator.output"));
}
fn1171036738();
--EXPECTF--
bool(false)
%unicode|string%(1) "&"

