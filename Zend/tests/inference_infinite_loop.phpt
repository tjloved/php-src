--TEST--
Type inference should not result in infinite loop
--FILE--
<?php

function test()
{
    $b = false;
    do {
        $a = $a + PHP_INT_MAX + 2;
        $a = 0;
    } while ($b);
}
function fn1625360197()
{
    test();
}
fn1625360197();
--EXPECTF--
Notice: Undefined variable: a in %s on line %d
