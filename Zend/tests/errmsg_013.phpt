--TEST--
errmsg: default value for parameters with array type can only be an array or NULL
--FILE--
<?php

class test
{
    function foo(array $a = "s")
    {
    }
}
function fn2144484425()
{
    echo "Done\n";
}
fn2144484425();
--EXPECTF--	
Fatal error: Default value for parameters with array type can only be an array or NULL in %s on line %d
