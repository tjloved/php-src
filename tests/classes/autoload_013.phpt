--TEST--
Ensure the ReflectionClass constructor triggers autoload.
--SKIPIF--
<?php extension_loaded('reflection') or die('skip'); ?>
--FILE--
<?php

function __autoload($name)
{
    echo "In autoload: ";
    var_dump($name);
}
function fn82954898()
{
    try {
        new ReflectionClass("UndefC");
    } catch (ReflectionException $e) {
        echo $e->getMessage();
    }
}
fn82954898();
--EXPECTF--
In autoload: string(6) "UndefC"
Class UndefC does not exist
