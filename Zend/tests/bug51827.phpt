--TEST--
Bug #51827 (Bad warning when register_shutdown_function called with wrong num of parameters)
--FILE--
<?php

function abc()
{
    var_dump(1);
}
function fn314667363()
{
    register_shutdown_function('timE');
    register_shutdown_function('ABC');
    register_shutdown_function('exploDe');
}
fn314667363();
--EXPECTF--
int(1)

Warning: explode() expects at least 2 parameters, 0 given in Unknown on line %d
