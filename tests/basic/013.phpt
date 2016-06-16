--TEST--
POST Method test and arrays
--POST--
a[]=1
--FILE--
<?php

function fn816433357()
{
    var_dump($_POST['a']);
}
fn816433357();
--EXPECT--
array(1) {
  [0]=>
  string(1) "1"
}
