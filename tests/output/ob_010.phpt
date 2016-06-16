--TEST--
output buffering - fatalism
--FILE--
<?php

function obh($s)
{
    return print_r($s, 1);
}
function fn1509026903()
{
    ob_start("obh");
    echo "foo\n";
}
fn1509026903();
--EXPECTF--
foo
