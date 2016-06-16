--TEST--
Trying to use lambda as array key
--FILE--
<?php

function fn773671150()
{
    var_dump(array(function () {
    } => 1));
}
fn773671150();
--EXPECTF--
Warning: Illegal offset type in %s on line %d
array(0) {
}
