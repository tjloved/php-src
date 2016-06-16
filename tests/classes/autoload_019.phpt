--TEST--
Ensure __autoload() recursion is guarded for multiple lookups of same class using difference case.
--FILE--
<?php

function __autoload($name)
{
    echo __FUNCTION__ . " {$name}\n";
    class_exists("undefinedCLASS");
}
function fn1977532594()
{
    class_exists("unDefinedClass");
}
fn1977532594();
--EXPECTF--
__autoload unDefinedClass
