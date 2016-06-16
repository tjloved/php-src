--TEST--
Array addition is not commutative -- do not swap operands
--FILE--
<?php

function fn540130950()
{
    $array = [1, 2, 3];
    $array = [4, 5, 6] + $array;
    var_dump($array);
}
fn540130950();
--EXPECT--
array(3) {
  [0]=>
  int(4)
  [1]=>
  int(5)
  [2]=>
  int(6)
}
