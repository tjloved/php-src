--TEST--
Testing Closure::fromCallable() functionality: Late static binding
--FILE--
<?php

class A
{
    public static function test()
    {
        var_dump(static::class);
    }
}
class B extends A
{
}
function fn1527313198()
{
    Closure::fromCallable(['A', 'test'])();
    Closure::fromCallable(['B', 'test'])();
}
fn1527313198();
--EXPECT--
string(1) "A"
string(1) "B"
