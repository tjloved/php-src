--TEST--
Testing function parameter passing with a return value
--FILE--
<?php

function test($b)
{
    $b++;
    return $b;
}
function fn1439557644()
{
    $a = test(1);
    echo $a;
}
fn1439557644();
--EXPECT--
2
