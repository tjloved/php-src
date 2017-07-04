--TEST--
Traits with static methods.
--CREDITS--
Simas Toleikis simast@gmail.com
--FILE--
<?php

trait TestTrait
{
    public static function test()
    {
        return 'Test';
    }
}
class A
{
    use TestTrait;
}
function fn964636976()
{
    echo A::test();
}
fn964636976();
--EXPECT--
Test