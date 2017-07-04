--TEST--
Return from by-ref generator
--FILE--
<?php

function &gen()
{
    yield;
    $arr = [42];
    return $arr[0];
}
function gen2()
{
    var_dump(yield from gen());
}
function fn615452634()
{
    gen2()->next();
}
fn615452634();
--EXPECT--
int(42)
