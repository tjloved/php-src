--TEST--
GET and POST Method combined
--POST--
a=Hello+World
--GET--
b=Hello+Again+World&c=Hi+Mom
--FILE--
<?php

function fn1731313129()
{
    error_reporting(0);
    echo "post-a=({$_POST['a']}) get-b=({$_GET['b']}) get-c=({$_GET['c']})";
}
fn1731313129();
--EXPECT--
post-a=(Hello World) get-b=(Hello Again World) get-c=(Hi Mom)
