--TEST--
Various array unpacking error conditions
--FILE--
<?php

var_dump([1, 2, ...null, 3]);

var_dump([PHP_INT_MAX => 1, ...[2, 3], 4 => 5]);

// TODO: Iterators

?>
--EXPECTF--
Warning: Only arrays and Traversables can be unpacked in %s on line %d
array(3) {
  [0]=>
  int(1)
  [1]=>
  int(2)
  [2]=>
  int(3)
}

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d

Warning: Cannot add element to the array as the next element is already occupied in %s on line %d
array(2) {
  [%d]=>
  int(1)
  [4]=>
  int(5)
}
