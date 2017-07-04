--TEST--
Bug #73960: Leak with instance method calling static method with referenced return
--FILE--
<?php

function fn1617615690()
{
    $value = 'one';
    $array = array($value);
    $array = $ref =& $array;
    var_dump($array);
}
fn1617615690();
--EXPECT--
array(1) {
  [0]=>
  string(3) "one"
}
