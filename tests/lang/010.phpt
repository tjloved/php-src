--TEST--
Testing function parameter passing with a return value
--FILE--
<?php

function test($b)
{
    $b++;
    return $b;
}
function fn136345321()
{
    $a = test(1);
    echo $a;
}
fn136345321();
--EXPECT--
2
