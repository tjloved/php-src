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
function fn398403798()
{
    var_dump(B::foo());
}
fn398403798();
--EXPECT--
string(1) "A"
