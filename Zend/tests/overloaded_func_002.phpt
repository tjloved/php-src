--TEST--
Overloaded function 002
--SKIPIF--
<?php
if (!class_exists('_ZendTestClass')) die("skip needs class with overloaded function");
?>
--FILE--
<?php

function fn983996755()
{
    $a = new _ZendTestClass();
    var_dump($a->{trim(" test")}());
}
fn983996755();
--EXPECT--
string(4) "test"
