--TEST--
Constant propagation 004
--FILE--
<?php

function test($b)
{
    if ($b) {
        $x = 1;
    }
    var_dump($x);
}
function fn2069956400()
{
    test(false);
}
fn2069956400();
--EXPECTF--
Notice: Undefined variable: x in %s on line %d
NULL
