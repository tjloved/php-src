--TEST--
errmsg: __isset() must take exactly 1 argument
--FILE--
<?php

class test
{
    function __isset()
    {
    }
}
function fn1174058278()
{
    echo "Done\n";
}
fn1174058278();
--EXPECTF--	
Fatal error: Method test::__isset() must take exactly 1 argument in %s on line %d
