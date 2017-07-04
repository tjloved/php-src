--TEST--
redefining constructor (__construct second)
--FILE--
<?php

class test
{
    function test()
    {
    }
    function __construct()
    {
    }
}
function fn2139146566()
{
    echo "Done\n";
}
fn2139146566();
--EXPECTF--	
Done
