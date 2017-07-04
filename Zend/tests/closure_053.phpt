--TEST--
Closure 053: self::class in static closure in non-static method.

--FILE--
<?php

class A
{
    function foo()
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
function fn277794822()
{
    $b = new B();
    var_dump($b->foo());
}
fn277794822();
--EXPECT--
string(1) "A"
