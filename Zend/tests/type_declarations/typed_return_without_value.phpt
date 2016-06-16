--TEST--
Typed return without value generates compile-time error
--FILE--
<?php

function test() : int
{
    return;
}
function fn960840852()
{
    test();
}
fn960840852();
--EXPECTF--
Fatal error: A function with return type must return a value in %s on line %d
