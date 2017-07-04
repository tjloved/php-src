--TEST--
Scalar type - disallow relative types
--FILE--
<?php

function foo(bar\int $a) : int
{
    return $a;
}
function fn55564645()
{
    foo(10);
}
fn55564645();
--EXPECTF--
Fatal error: Cannot use 'bar\int' as class name as it is reserved in %s on line %d
