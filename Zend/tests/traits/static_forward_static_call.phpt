--TEST--
Traits and forward_static_call().
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

trait TestTrait
{
    public static function test()
    {
        return 'Forwarded ' . forward_static_call(array('A', 'test'));
    }
}
class A
{
    public static function test()
    {
        return "Test A";
    }
}
class B extends A
{
    use TestTrait;
}
function fn424048159()
{
    echo B::test();
}
fn424048159();
--EXPECT--
Forwarded Test A