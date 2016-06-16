--TEST--
INI section allows '='
--INI--
arg_separator.input==
--FILE--
<?php

function fn661139476()
{
    var_dump(ini_get('arg_separator.input'));
}
fn661139476();
--EXPECT--
string(1) "="
