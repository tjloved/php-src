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
function fn807871568()
{
    echo "Done\n";
}
fn807871568();
--EXPECTF--	
Fatal error: Cannot redeclare test::foo() in %s on line %d
