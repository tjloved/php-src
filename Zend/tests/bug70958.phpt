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
function fn224842639()
{
    $b = new B();
    $b->bar();
}
fn224842639();
--EXPECT--
string(1) "B"
