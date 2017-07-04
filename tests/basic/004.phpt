--TEST--
Two variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World
--FILE--
<?php

function fn216260378()
{
    error_reporting(0);
    echo "{$_POST['a']} {$_POST['b']}";
}
fn216260378();
--EXPECT--
Hello World Hello Again World
