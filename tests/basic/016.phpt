--TEST--
POST Method test and arrays - 4 
--POST--
a[a]=1&a[b]=3
--FILE--
<?php

function fn1454553006()
{
    var_dump($_POST['a']);
}
fn1454553006();
--EXPECT--
array(2) {
  ["a"]=>
  string(1) "1"
  ["b"]=>
  string(1) "3"
}
