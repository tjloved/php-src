--TEST--
Bug #37251 (deadlock when custom error handler is to catch array type hint error) 
--FILE--
<?php

class Foo
{
    function bar(array $foo)
    {
    }
}
function fn1487606354()
{
    try {
        $foo = new Foo();
        $foo->bar();
    } catch (Error $e) {
        echo 'OK';
    }
}
fn1487606354();
--EXPECT--
OK
