--TEST--
Bug #71539 (Memory error on $arr[$a] =& $arr[$b] if RHS rehashes)
--FILE--
<?php

function fn298147708()
{
    $array = [];
    $array[0] =& $array[''];
    $array[0] = 42;
    var_dump($array);
}
fn298147708();
--EXPECT--
array(2) {
  [""]=>
  &int(42)
  [0]=>
  &int(42)
}
