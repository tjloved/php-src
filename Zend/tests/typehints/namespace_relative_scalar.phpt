--TEST--
namespace\int is not a valid type hint
--FILE--
<?php

function test(namespace\int $i)
{
}
function fn30708653()
{
    test(0);
}
fn30708653();
--EXPECTF--
Fatal error: Scalar type declaration 'int' must be unqualified in %s on line %d
