--TEST--
Tests that array manipulation code is correctly dealing with copy on write and splitting on reference
--FILE--
<?php

function fn876863175()
{
    $a = array();
    $b = 1;
    $c =& $b;
    $a[] = $b;
    $b = 2;
    var_dump($a);
}
fn876863175();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
