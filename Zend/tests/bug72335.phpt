--TEST--
Misoptimize due to type narrowing
--FILE--
<?php

function test()
{
    $b = false;
    $x = (1 << 53) + 1;
    do {
        $x = 1.0 * ($x - (1 << 53));
    } while ($b);
    return $x;
}
function fn2104268829()
{
    var_dump(test());
}
fn2104268829();
--EXPECT--
float(1)
