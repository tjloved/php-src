--TEST--
Ensure instanceof does not trigger autoload.
--FILE--
<?php

function fn1152868734()
{
    spl_autoload_register(function ($name) {
        echo "In autoload: ";
        var_dump($name);
    });
    $a = new stdClass();
    var_dump($a instanceof UndefC);
}
fn1152868734();
--EXPECTF--
bool(false)
