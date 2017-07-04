--TEST--
POST Method test and arrays - 7 
--POST--
a[]=1&a[]]=3&a[[]=4
--FILE--
<?php

function fn924208339()
{
    var_dump($_POST['a']);
}
fn924208339();
--EXPECT--
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "3"
  ["["]=>
  string(1) "4"
}
