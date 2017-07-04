--TEST--
Ensure the ReflectionProperty constructor triggers autoload.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

function fn1426792907()
{
    spl_autoload_register(function ($name) {
        echo "In autoload: ";
        var_dump($name);
    });
    try {
        new ReflectionProperty('UndefC', 'p');
    } catch (ReflectionException $e) {
        echo $e->getMessage();
    }
}
fn1426792907();
--EXPECTF--
In autoload: string(6) "UndefC"
Class UndefC does not exist
