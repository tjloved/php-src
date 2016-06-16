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
function fn537421441()
{
    echo "Done\n";
}
fn537421441();
--EXPECTF--	
Fatal error: Method test::__isset() must take exactly 1 argument in %s on line %d
