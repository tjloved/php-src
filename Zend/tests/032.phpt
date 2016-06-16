--TEST--
Testing array with '[]' passed as argument by reference
--FILE--
<?php

function test(&$var)
{
}
function fn1721236399()
{
    test($arr[]);
    print "ok!\n";
}
fn1721236399();
--EXPECT--
ok!
