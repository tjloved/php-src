--TEST--
errmsg: can't use function return value in write context
--FILE--
<?php

function foo()
{
    return "blah";
}
function fn772731734()
{
    foo() = 1;
    echo "Done\n";
}
fn772731734();
--EXPECTF--	
Fatal error: Can't use function return value in write context in %s on line %d
