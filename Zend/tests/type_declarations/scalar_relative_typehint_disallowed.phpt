--TEST--
Scalar type - disallow relative types
--FILE--
<?php

function foo(bar\int $a) : int
{
    return $a;
}
function fn1719831298()
{
    foo(10);
}
fn1719831298();
--EXPECTF--
Fatal error: Cannot use 'bar\int' as class name as it is reserved in %s on line %d
