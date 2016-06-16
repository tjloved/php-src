--TEST--
Generators can yield by-reference
--FILE--
<?php

function &iter(array &$array)
{
    foreach ($array as $key => &$value) {
        (yield $key => $value);
    }
}
function fn1961160841()
{
    $array = [1, 2, 3];
    $iter = iter($array);
    foreach ($iter as &$value) {
        $value *= -1;
    }
    var_dump($array);
    $array = [1, 2, 3];
    foreach (iter($array) as &$value) {
        $value *= -1;
    }
    var_dump($array);
}
fn1961160841();
--EXPECT--
array(3) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  &int(-3)
}
array(3) {
  [0]=>
  int(-1)
  [1]=>
  int(-2)
  [2]=>
  &int(-3)
}
