--TEST--
Bug #38469 (Unexpected creation of cycle)
--FILE--
<?php

function f()
{
    $a = array();
    $a[0] = $a;
    var_dump($a);
    $b = array(array());
    $b[0][0] = $b;
    var_dump($b);
}
function fn682713622()
{
    $a = array();
    $a[0] = $a;
    var_dump($a);
    $b = array(array());
    $b[0][0] = $b;
    var_dump($b);
    f();
}
fn682713622();
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
}
array(1) {
  [0]=>
  array(0) {
  }
}
array(1) {
  [0]=>
  array(1) {
    [0]=>
    array(1) {
      [0]=>
      array(0) {
      }
    }
  }
}
