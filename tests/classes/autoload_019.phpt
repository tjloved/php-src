--TEST--
Ensure __autoload() recursion is guarded for multiple lookups of same class using difference case.
--FILE--
<?php

function fn974999960()
{
    spl_autoload_register(function ($name) {
        echo "autoload {$name}\n";
        class_exists("undefinedCLASS");
    });
    class_exists("unDefinedClass");
}
fn974999960();
--EXPECTF--
autoload unDefinedClass
