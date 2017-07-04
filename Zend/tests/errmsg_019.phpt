--TEST--
errmsg: __destruct() cannot take arguments 
--FILE--
<?php

class test
{
    function __destruct($var)
    {
    }
}
function fn2027751296()
{
    echo "Done\n";
}
fn2027751296();
--EXPECTF--	
Fatal error: Destructor test::__destruct() cannot take arguments in %s on line %d
