--TEST--
Bug #72543.2 (different references behavior comparing to PHP 5)
--FILE--
<?php

function fn81074848()
{
    $arr = [];
    $arr[0] = null;
    $ref =& $arr[0];
    unset($ref);
    $arr[0][$arr[0]] = null;
    var_dump($arr);
}
fn81074848();
--EXPECT--
array(1) {
  [0]=>
  array(1) {
    [""]=>
    NULL
  }
}
