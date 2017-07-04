--TEST--
POST Method test and arrays - 2
--POST--
a[]=1&a[]=1
--FILE--
<?php

function fn348950201()
{
    var_dump($_POST['a']);
}
fn348950201();
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
