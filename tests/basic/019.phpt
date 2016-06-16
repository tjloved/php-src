--TEST--
POST Method test and arrays - 7 
--POST--
a[]=1&a[]]=3&a[[]=4
--FILE--
<?php

function fn1680989443()
{
    var_dump($_POST['a']);
}
fn1680989443();
--EXPECT--
array(3) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "3"
  ["["]=>
  string(1) "4"
}
