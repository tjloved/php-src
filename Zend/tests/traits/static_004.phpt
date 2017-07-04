--TEST--
Traits with __callStatic magic method.
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

trait TestTrait
{
    public static function __callStatic($name, $arguments)
    {
        return $name;
    }
}
class A
{
    use TestTrait;
}
function fn134262243()
{
    echo A::Test();
}
fn134262243();
--EXPECT--
Test