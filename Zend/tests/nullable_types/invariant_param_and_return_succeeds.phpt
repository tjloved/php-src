--TEST--
Invariant parameter and return types work with nullable types

--FILE--
<?php

interface A
{
    function method(?int $i) : ?int;
}
class B implements A
{
    function method(?int $i) : ?int
    {
        return $i;
    }
}
function fn2050880412()
{
    $b = new B();
    var_dump($b->method(null));
    var_dump($b->method(1));
}
fn2050880412();
--EXPECT--
NULL
int(1)

