--TEST--
Closure 049: static::class in static closure in non-static method.

--FILE--
<?php

class A
{
    function foo()
    {
        $f = static function () {
            return static::class;
        };
        return $f();
    }
}
class B extends A
{
}
function fn444290176()
{
    $b = new B();
    var_dump($b->foo());
}
fn444290176();
--EXPECT--
string(1) "B"
