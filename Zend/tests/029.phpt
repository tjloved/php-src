--TEST--
Testing assign to property of an object in an array
--FILE--
<?php

function fn1676319472()
{
    $arr = array(new stdClass());
    $arr[0]->a = clone $arr[0];
    var_dump($arr);
    $arr[0]->b = new $arr[0]();
    var_dump($arr);
    $arr[0]->c = $arr[0]->a;
    var_dump($arr);
}
fn1676319472();
--EXPECT--
array(1) {
  [0]=>
  object(stdClass)#1 (1) {
    ["a"]=>
    object(stdClass)#2 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(stdClass)#1 (2) {
    ["a"]=>
    object(stdClass)#2 (0) {
    }
    ["b"]=>
    object(stdClass)#3 (0) {
    }
  }
}
array(1) {
  [0]=>
  object(stdClass)#1 (3) {
    ["a"]=>
    object(stdClass)#2 (0) {
    }
    ["b"]=>
    object(stdClass)#3 (0) {
    }
    ["c"]=>
    object(stdClass)#2 (0) {
    }
  }
}
