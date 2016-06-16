--TEST--
Closure 051: static::class in static closure in static method.

--FILE--
<?php

class A
{
    static function foo()
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
function fn561102694()
{
    var_dump(B::foo());
}
fn561102694();
--EXPECT--
string(1) "B"
