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
function fn790081013()
{
    echo "Done\n";
}
fn790081013();
--EXPECT--
Done
