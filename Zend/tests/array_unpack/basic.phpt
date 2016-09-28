--TEST--
Basic array unpacking functionality
--FILE--
<?php

var_dump(['x' => 'y', ...['a' => 'b', 'c' => 'd'], 'z' => 'u']);
var_dump([0, ...[1, 2], 3]);
var_dump([42 => 0, ...[123 => 1, 124 => 2], 3]);

var_dump(['x' => 'y', ...new ArrayIterator(['a' => 'b', 'c' => 'd']), 'z' => 'u']);
var_dump([0, ...new ArrayIterator([1, 2]), 3]);
var_dump([42 => 0, ...new ArrayIterator([123 => 1, 124 => 2]), 3]);

$arr = [1, 2];
$ref =& $arr;
var_dump([0, ...$ref, 3]);

?>
--EXPECT--
array(4) {
  ["x"]=>
  string(1) "y"
  ["a"]=>
  string(1) "b"
  ["c"]=>
  string(1) "d"
  ["z"]=>
  string(1) "u"
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(4) {
  [42]=>
  int(0)
  [43]=>
  int(1)
  [44]=>
  int(2)
  [45]=>
  int(3)
}
array(4) {
  ["x"]=>
  string(1) "y"
  ["a"]=>
  string(1) "b"
  ["c"]=>
  string(1) "d"
  ["z"]=>
  string(1) "u"
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
array(4) {
  [42]=>
  int(0)
  [43]=>
  int(1)
  [44]=>
  int(2)
  [45]=>
  int(3)
}
array(4) {
  [0]=>
  int(0)
  [1]=>
  int(1)
  [2]=>
  int(2)
  [3]=>
  int(3)
}
