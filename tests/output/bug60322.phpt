--TEST--
Bug #60322 (ob_get_clean() now raises an E_NOTICE if no buffers exist)
--INI--
output_buffering=128
--FILE--
<?php

function fn1789982509()
{
    ob_start();
    while (@ob_end_clean()) {
    }
    var_dump(ob_get_clean());
}
fn1789982509();
--EXPECT--
bool(false)
