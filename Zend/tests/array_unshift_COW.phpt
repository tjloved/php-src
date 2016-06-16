--TEST--
Tests that array unshift code is correctly dealing with copy on write and splitting on reference
--FILE--
<?php

function fn2096793220()
{
    $a = array();
    $b = 1;
    $c =& $b;
    array_unshift($a, $b);
    $b = 2;
    var_dump($a);
}
fn2096793220();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
