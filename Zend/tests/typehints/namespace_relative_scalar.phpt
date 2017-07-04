--TEST--
namespace\int is not a valid type hint
--FILE--
<?php

function test(namespace\int $i)
{
}
function fn371856290()
{
    test(0);
}
fn371856290();
--EXPECTF--
Fatal error: Scalar type declaration 'int' must be unqualified in %s on line %d
