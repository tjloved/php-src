--TEST--
Subtype can add nullability to a parameter (contravariance)

--FILE--
<?php

interface A
{
    function method(int $p);
}
class B implements A
{
    function method(?int $p)
    {
    }
}
function fn617450165()
{
    $b = new B();
    $b->method(null);
}
fn617450165();
--EXPECT--

