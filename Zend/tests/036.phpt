--TEST--
Trying to use lambda in array offset
--FILE--
<?php

function fn948174692()
{
    $test[function () {
    }] = 1;
    $a[function () {
    }] = 1;
}
fn948174692();
--EXPECTF--
Warning: Illegal offset type in %s on line %d

Warning: Illegal offset type in %s on line %d
