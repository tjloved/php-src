--TEST--
Ensure ReflectionClass::implementsInterface triggers autoload.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

function fn1416685805()
{
    spl_autoload_register(function ($name) {
        echo "In autoload: ";
        var_dump($name);
    });
    $rc = new ReflectionClass("stdClass");
    try {
        $rc->implementsInterface("UndefI");
    } catch (ReflectionException $e) {
        echo $e->getMessage();
    }
}
fn1416685805();
--EXPECTF--
In autoload: string(6) "UndefI"
Interface UndefI does not exist
