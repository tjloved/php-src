--TEST--
Traits and get_called_class().
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

trait TestTrait
{
    public static function test()
    {
        return get_called_class();
    }
}
class A
{
    use TestTrait;
}
class B extends A
{
}
function fn80782283()
{
    echo B::test();
}
fn80782283();
--EXPECT--
B