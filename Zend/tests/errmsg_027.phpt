--TEST--
errmsg: class declarations may not be nested
--FILE--
<?php

class test
{
    function foo()
    {
        class test2
        {
        }
    }
}
function fn1201218403()
{
    echo "Done\n";
}
fn1201218403();
--EXPECTF--	
Fatal error: Class declarations may not be nested in %s on line %d
