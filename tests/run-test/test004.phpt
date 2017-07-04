--TEST--
INI section allows '='
--INI--
arg_separator.input==
--FILE--
<?php

function fn1861052195()
{
    var_dump(ini_get('arg_separator.input'));
}
fn1861052195();
--EXPECT--
string(1) "="
