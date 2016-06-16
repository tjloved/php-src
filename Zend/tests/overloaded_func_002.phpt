--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!PHP_DEBUG) die("skip only run in debug version");
?>
--FILE--
<?php

function fn1533956941()
{
    $a = new _ZendTestClass();
    var_dump($a->{trim(" test")}());
}
fn1533956941();
--EXPECT--
string(4) "test"
