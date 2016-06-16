--TEST--
ini_alter() check
--CREDITS--
Sebastian Schürmann
sebs@php.net
Testfest 2009 Munich
--FILE--
<?php

function fn1712442147()
{
    ini_alter('error_reporting', 1);
    $var = ini_get('error_reporting');
    var_dump($var);
    ini_alter('error_reporting', 0);
    $var = ini_get('error_reporting');
    var_dump($var);
}
fn1712442147();
--EXPECT--
string(1) "1"
string(1) "0"

