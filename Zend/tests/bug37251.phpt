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
function fn530611356()
{
    try {
        $foo = new Foo();
        $foo->bar();
    } catch (Error $e) {
        echo 'OK';
    }
}
fn530611356();
--EXPECT--
OK
