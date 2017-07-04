--TEST--
Bug #72441 (Segmentation fault: RFC list_keys)
--FILE--
<?php

function fn1059446741()
{
    $array = [];
    list('' => $foo, $bar) = $array;
}
fn1059446741();
--EXPECTF--
Fatal error: Cannot mix keyed and unkeyed array entries in assignments in %sbug72441.php on line %d
