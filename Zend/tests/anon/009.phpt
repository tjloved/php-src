--TEST--
testing traits in anon classes
--FILE--
<?php

trait Foo
{
    public function someMethod()
    {
        return "bar";
    }
}
function fn1531467604()
{
    $anonClass = new class
    {
        use Foo;
    };
    var_dump($anonClass->someMethod());
}
fn1531467604();
--EXPECT--
string(3) "bar"
