--TEST--
Yielding the result of a non-ref function call throw a notice
--FILE--
<?php

function foo()
{
    return "bar";
}
function &gen()
{
    (yield foo());
}
function fn1104867790()
{
    $gen = gen();
    var_dump($gen->current());
}
fn1104867790();
--EXPECTF--
Notice: Only variable references should be yielded by reference in %s on line %d
string(3) "bar"
