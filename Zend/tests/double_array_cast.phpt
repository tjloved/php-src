--TEST--
Double array cast
--FILE--
<?php

function fn134537420()
{
    $array = [1, 2, $x = 3];
    var_dump((array) (array) $array);
}
fn134537420();
--EXPECT--
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}
