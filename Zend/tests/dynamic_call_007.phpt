--TEST--
Dynamic calls to scope introspection functions are forbidden (misoptimization)
--FILE--
<?php

function test()
{
    $i = 1;
    array_map('extract', [['i' => new stdClass()]]);
    $i += 1;
    var_dump($i);
}
function fn1874382993()
{
    test();
}
fn1874382993();
--EXPECTF--
Warning: Cannot call extract() dynamically in %s on line %d
int(2)
