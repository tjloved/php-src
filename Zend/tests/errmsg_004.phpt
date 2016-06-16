--TEST--
errmsg: can't use function return value in write context
--FILE--
<?php

function foo()
{
    return "blah";
}
function fn1085400539()
{
    foo() = 1;
    echo "Done\n";
}
fn1085400539();
--EXPECTF--	
Fatal error: Can't use function return value in write context in %s on line %d
