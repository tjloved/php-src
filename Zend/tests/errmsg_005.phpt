--TEST--
errmsg: can't use method return value in write context
--FILE--
<?php

class test
{
    function foo()
    {
        return "blah";
    }
}
function fn1197388939()
{
    $t = new test();
    $t->foo() = 1;
    echo "Done\n";
}
fn1197388939();
--EXPECTF--	
Fatal error: Can't use method return value in write context in %s on line %d
