--TEST--
Bug #67169: array_splice all elements, then []= gives wrong index
--FILE--
<?php

function fn2121057027()
{
    $array = array('a', 'b');
    array_splice($array, 0, 2);
    $array[] = 'c';
    var_dump($array);
    $array = array('a', 'b');
    array_shift($array);
    array_shift($array);
    $array[] = 'c';
    var_dump($array);
}
fn2121057027();
--EXPECT--
array(1) {
  [0]=>
  string(1) "c"
}
array(1) {
  [0]=>
  string(1) "c"
}
