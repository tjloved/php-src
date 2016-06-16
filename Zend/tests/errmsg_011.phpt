--TEST--
errmsg: cannot redeclare (method)
--FILE--
<?php

class test
{
    function foo()
    {
    }
    function foo()
    {
    }
}
function fn207369307()
{
    echo "Done\n";
}
fn207369307();
--EXPECTF--	
Fatal error: Cannot redeclare test::foo() in %s on line %d
