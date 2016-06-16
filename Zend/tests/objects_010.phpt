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
function fn1556450122()
{
    echo "Done\n";
}
fn1556450122();
--EXPECTF--	
Done
