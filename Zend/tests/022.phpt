--TEST--
Implementating abstracting methods and optional parameters
--FILE--
<?php

abstract class Base
{
    abstract function someMethod($param);
}
class Ext extends Base
{
    function someMethod($param = "default")
    {
        echo $param, "\n";
    }
}
function fn584476222()
{
    $a = new Ext();
    $a->someMethod("foo");
    $a->someMethod();
}
fn584476222();
--EXPECT--
foo
default
