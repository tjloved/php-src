--TEST--
Trying to override final method
--FILE--
<?php

trait foo
{
    public function test()
    {
        return 3;
    }
}
class baz
{
    public final function test()
    {
        return 4;
    }
}
class bar extends baz
{
    use foo {
        test as public;
    }
}
function fn1476651467()
{
    $x = new bar();
    var_dump($x->test());
}
fn1476651467();
--EXPECTF--
Fatal error: Cannot override final method baz::test() in %s on line %d
