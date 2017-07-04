--TEST--
Closure 055: self::class in static closure in static method.

--FILE--
<?php

class A
{
    static function foo()
    {
        $f = static function () {
            return self::class;
        };
        return $f();
    }
}
class B extends A
{
}
function fn1575812856()
{
    var_dump(B::foo());
}
fn1575812856();
--EXPECT--
string(1) "A"
