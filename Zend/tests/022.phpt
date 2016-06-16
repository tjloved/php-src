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
function fn485407354()
{
    $a = new Ext();
    $a->someMethod("foo");
    $a->someMethod();
}
fn485407354();
--EXPECT--
foo
default
