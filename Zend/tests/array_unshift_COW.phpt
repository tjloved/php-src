--TEST--
Tests that array unshift code is correctly dealing with copy on write and splitting on reference
--FILE--
<?php

function fn890436946()
{
    $a = array();
    $b = 1;
    $c =& $b;
    array_unshift($a, $b);
    $b = 2;
    var_dump($a);
}
fn890436946();
--EXPECT--
array(1) {
  [0]=>
  int(1)
}
