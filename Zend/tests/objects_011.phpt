--TEST--
redefining constructor (__construct first)
--INI--
error_reporting=8191
--FILE--
<?php

class test
{
    function __construct()
    {
    }
    function test()
    {
    }
}
function fn2122140847()
{
    echo "Done\n";
}
fn2122140847();
--EXPECT--
Done
