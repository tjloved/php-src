--TEST--
Fully qualified (leading backslash) type names must fail
--FILE--
<?php

function foo(\int $foo)
{
    var_dump($foo);
}
function fn180230082()
{
    foo(1);
}
fn180230082();
--EXPECTF--
Fatal error: Scalar type declaration 'int' must be unqualified in %s on line %d
