--TEST--
Testing function parameter passing
--FILE--
<?php

function test($a, $b)
{
    echo $a + $b;
}
function fn417147811()
{
    test(1, 2);
}
fn417147811();
--EXPECT--
3
