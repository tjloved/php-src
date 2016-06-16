--TEST--
Testing each() with array and object
--FILE--
<?php

function fn1130183819()
{
    $a = new stdClass();
    $foo = each($a);
    var_dump($foo);
    $a = new stdClass();
    var_dump(each($a));
    $a = array(new stdClass());
    var_dump(each($a));
}
fn1130183819();
--EXPECTF--
bool(false)
bool(false)
array(4) {
  [1]=>
  object(stdClass)#1 (0) {
  }
  ["value"]=>
  object(stdClass)#1 (0) {
  }
  [0]=>
  int(0)
  ["key"]=>
  int(0)
}
