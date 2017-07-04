--TEST--
Bug #71539.3 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php

function fn1218262287()
{
    $array = [];
    $array[0][0] =& $array[''];
    $array[0][0] = 42;
    var_dump($array);
}
fn1218262287();
--EXPECT--
array(2) {
  [""]=>
  &int(42)
  [0]=>
  array(1) {
    [0]=>
    &int(42)
  }
}
