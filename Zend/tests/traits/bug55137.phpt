--TEST--
Bug #55137 (Changing trait static method visibility)
--FILE--
<?php

trait A
{
    protected static function foo()
    {
        echo "abc\n";
    }
    private static function bar()
    {
        echo "def\n";
    }
}
class B
{
    use A {
        A::foo as public;
        A::bar as public baz;
    }
}
function fn991186358()
{
    B::foo();
    B::baz();
}
fn991186358();
--EXPECT--
abc
def
