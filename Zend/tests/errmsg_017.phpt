--TEST--
errmsg: __unset() must take exactly 1 argument 
--FILE--
<?php

class test
{
    function __unset()
    {
    }
}
function fn1957245019()
{
    echo "Done\n";
}
fn1957245019();
--EXPECTF--	
Fatal error: Method test::__unset() must take exactly 1 argument in %s on line %d
