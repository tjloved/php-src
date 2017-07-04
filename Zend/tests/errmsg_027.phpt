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
function fn597119408()
{
    echo "Done\n";
}
fn597119408();
--EXPECTF--	
Fatal error: Class declarations may not be nested in %s on line %d
