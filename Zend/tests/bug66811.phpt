--TEST--
Bug #66811: Cannot access static::class in lambda, writen outside of a class
--FILE--
<?php

class A
{
    public static function f()
    {
        return function () {
            var_dump(self::class);
            var_dump(static::class);
        };
    }
}
class B extends A
{
}
function fn1232772862()
{
    $f = B::f();
    $f();
    $g = $f->bindTo(null, A::class);
    $g();
    $foo = function () {
        var_dump(self::class);
        var_dump(static::class);
    };
    $bar = $foo->bindTo(null, A::class);
    $bar();
}
fn1232772862();
--EXPECT--
string(1) "A"
string(1) "B"
string(1) "A"
string(1) "A"
string(1) "A"
string(1) "A"
