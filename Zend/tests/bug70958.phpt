--TEST--
Bug #70958 (Invalid opcode while using ::class as trait method paramater default value)
--FILE--
<?php

trait Foo
{
    function bar($a = self::class)
    {
        var_dump($a);
    }
}
class B
{
    use Foo;
}
function fn195200440()
{
    $b = new B();
    $b->bar();
}
fn195200440();
--EXPECT--
string(1) "B"
