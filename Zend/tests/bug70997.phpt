--TEST--
Bug #70997 (When using parentClass:: instead of parent::, static context changed)
--FILE--
<?php

class A
{
    const TEST = false;
    public function test()
    {
        var_dump(static::TEST);
    }
}
class B extends A
{
    const TEST = true;
    public function test()
    {
        A::test();
    }
}
function fn1024162268()
{
    $b = new B();
    $b->test();
}
fn1024162268();
--EXPECT--
bool(true)
