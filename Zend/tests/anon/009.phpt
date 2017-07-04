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
function fn318801987()
{
    $anonClass = new class
    {
        use Foo;
    };
    var_dump($anonClass->someMethod());
}
fn318801987();
--EXPECT--
string(3) "bar"
