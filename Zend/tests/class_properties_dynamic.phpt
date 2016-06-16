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
function fn2092881593()
{
    echo (new Foo())->bar;
}
fn2092881593();
--EXPECTF--
3
