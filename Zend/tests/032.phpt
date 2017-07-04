--TEST--
Testing array with '[]' passed as argument by reference
--FILE--
<?php

function test(&$var)
{
}
function fn1627185255()
{
    test($arr[]);
    print "ok!\n";
}
fn1627185255();
--EXPECT--
ok!
