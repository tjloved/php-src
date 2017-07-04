--TEST--
Class Property Expressions
--FILE--
<?php

class Foo
{
    const BAR = 1 << 0;
    const BAZ = 1 << 1;
    public $bar = self::BAR | self::BAZ;
}
function fn213611496()
{
    echo (new Foo())->bar;
}
fn213611496();
--EXPECTF--
3
