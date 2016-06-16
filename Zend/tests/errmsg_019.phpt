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
function fn331716852()
{
    echo "Done\n";
}
fn331716852();
--EXPECTF--	
Fatal error: Destructor test::__destruct() cannot take arguments in %s on line %d
