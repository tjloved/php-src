--TEST--
Typed return without value generates compile-time error
--FILE--
<?php

function test() : int
{
    return;
}
function fn1103523326()
{
    test();
}
fn1103523326();
--EXPECTF--
Fatal error: A function with return type must return a value in %s on line %d
