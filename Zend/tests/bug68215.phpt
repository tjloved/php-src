--TEST--
Bug #68215 (Behavior of foreach has changed)
--FILE--
<?php

function test(&$child, $entry)
{
    $i = 1;
    foreach ($child as $key => $fruit) {
        if (!is_numeric($key)) {
            $child[$i] = $fruit;
            unset($child[$key]);
            $i++;
        }
    }
}
function fn118719569()
{
    $arr = array('a' => array('a' => 'apple', 'b' => 'banana', 'c' => 'cranberry', 'd' => 'mango', 'e' => 'pineapple'), 'b' => array('a' => 'apple', 'b' => 'banana', 'c' => 'cranberry', 'd' => 'mango', 'e' => 'pineapple'), 'c' => 'cranberry', 'd' => 'mango', 'e' => 'pineapple');
    $i = 1;
    foreach ($arr as $key => $fruit) {
        $arr[$i] = $fruit;
        if (is_array($fruit)) {
            test($arr[$i], $fruit);
        }
        unset($arr[$key]);
        $i++;
    }
    var_dump($arr);
}
fn118719569();
--EXPECT--
array(5) {
  [1]=>
  array(5) {
    [1]=>
    string(5) "apple"
    [2]=>
    string(6) "banana"
    [3]=>
    string(9) "cranberry"
    [4]=>
    string(5) "mango"
    [5]=>
    string(9) "pineapple"
  }
  [2]=>
  array(5) {
    [1]=>
    string(5) "apple"
    [2]=>
    string(6) "banana"
    [3]=>
    string(9) "cranberry"
    [4]=>
    string(5) "mango"
    [5]=>
    string(9) "pineapple"
  }
  [3]=>
  string(9) "cranberry"
  [4]=>
  string(5) "mango"
  [5]=>
  string(9) "pineapple"
}
