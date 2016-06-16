--TEST--
Test to check static method calls syntax regression
--FILE--
<?php

class Foo
{
    public static function function()
    {
        echo __METHOD__, PHP_EOL;
    }
}
function fn1927859012()
{
    Foo::function();
    Foo::function();
    Foo::function();
    Foo::function();
    echo "\nDone\n";
}
fn1927859012();
--EXPECTF--

Foo::function
Foo::function
Foo::function
Foo::function

Done
