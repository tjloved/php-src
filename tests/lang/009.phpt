--TEST--
Testing function parameter passing
--FILE--
<?php

function test($a, $b)
{
    echo $a + $b;
}
function fn1405939356()
{
    test(1, 2);
}
fn1405939356();
--EXPECT--
3
