--TEST--
ZE2 Late Static Binding is_callable() and static::method()
--FILE--
<?php

class Test1
{
    static function test()
    {
        var_dump(is_callable("static::ok"));
        var_dump(is_callable(array("static", "ok")));
    }
}
class Test2 extends Test1
{
    static function ok()
    {
    }
}
function fn1556545189()
{
    Test1::test();
    Test2::test();
}
fn1556545189();
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
