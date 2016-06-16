--TEST--
Two variables in POST data
--POST--
a=Hello+World&b=Hello+Again+World
--FILE--
<?php

function fn678897115()
{
    error_reporting(0);
    echo "{$_POST['a']} {$_POST['b']}";
}
fn678897115();
--EXPECT--
Hello World Hello Again World
