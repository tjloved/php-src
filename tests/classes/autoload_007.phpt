--TEST--
Ensure instanceof does not trigger autoload.
--FILE--
<?php

function __autoload($name)
{
    echo "In autoload: ";
    var_dump($name);
}
function fn1844711219()
{
    $a = new stdClass();
    var_dump($a instanceof UndefC);
}
fn1844711219();
--EXPECTF--
bool(false)
