--TEST--
Trying to use lambda as array key
--FILE--
<?php

function fn836335758()
{
    var_dump(array(function () {
    } => 1));
}
fn836335758();
--EXPECTF--
Warning: Illegal offset type in %s on line %d
array(0) {
}
