--TEST--
errmsg: __clone() cannot accept any arguments
--FILE--
<?php

class test
{
    function __clone($var)
    {
    }
}
function fn1184308867()
{
    echo "Done\n";
}
fn1184308867();
--EXPECTF--	
Fatal error: Method test::__clone() cannot accept any arguments in %s on line %d
