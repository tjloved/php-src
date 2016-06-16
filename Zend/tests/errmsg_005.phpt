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
function fn181378190()
{
    $t = new test();
    $t->foo() = 1;
    echo "Done\n";
}
fn181378190();
--EXPECTF--	
Fatal error: Can't use method return value in write context in %s on line %d
