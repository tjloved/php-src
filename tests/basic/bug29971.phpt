--TEST--
Bug #29971 (variables_order behaviour)
--INI--
variables_order=GPC
--FILE--
<?php

function fn324526278()
{
    var_dump($_ENV, $_SERVER);
    var_dump(ini_get("variables_order"));
}
fn324526278();
--EXPECT--
array(0) {
}
array(0) {
}
string(3) "GPC"
