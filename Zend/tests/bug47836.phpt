--TEST--
Bug #47836 (array operator [] inconsistency when the array has PHP_INT_MAX index value)
--FILE--
<?php

function fn1168853753()
{
    $arr[PHP_INT_MAX] = 1;
    $arr[] = 2;
    var_dump($arr);
}
fn1168853753();
--EXPECTF--
Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(1) {
  [%d]=>
  int(1)
}
