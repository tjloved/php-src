--TEST--
Three variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World&c=1
--FILE--
<?php

function fn1461722488()
{
    error_reporting(0);
    echo "{$_POST['a']} {$_POST['b']} {$_POST['c']}";
}
fn1461722488();
--EXPECT--
Hello World Hello Again World 1
