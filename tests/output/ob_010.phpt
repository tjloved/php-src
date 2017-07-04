--TEST--
output buffering - fatalism
--FILE--
<?php

function obh($s)
{
    return print_r($s, 1);
}
function fn533859517()
{
    ob_start("obh");
    echo "foo\n";
}
fn533859517();
--EXPECTF--
foo
