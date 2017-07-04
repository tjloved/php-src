--TEST--
Trying to use lambda in array offset
--FILE--
<?php

function fn1732783041()
{
    $test[function () {
    }] = 1;
    $a[function () {
    }] = 1;
}
fn1732783041();
--EXPECTF--
Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d
