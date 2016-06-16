--TEST--
POST Method test and arrays - 2
--POST--
a[]=1&a[]=1
--FILE--
<?php

function fn936888667()
{
    var_dump($_POST['a']);
}
fn936888667();
--EXPECT--
array(2) {
  [0]=>
  string(1) "1"
  [1]=>
  string(1) "1"
}
