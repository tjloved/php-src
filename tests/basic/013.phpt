--TEST--
POST Method test and arrays
--POST--
a[]=1
--FILE--
<?php

function fn1996423748()
{
    var_dump($_POST['a']);
}
fn1996423748();
--EXPECT--
array(1) {
  [0]=>
  string(1) "1"
}
