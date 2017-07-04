--TEST--
Testing array dereference and references
--FILE--
<?php

function &foo(&$foo)
{
    return $foo;
}
function fn1640923731()
{
    error_reporting(E_ALL);
    $a = array(1);
    foo($a)[0] = 2;
    var_dump($a);
    foo($a)[] = 3;
    var_dump($a);
}
fn1640923731();
--EXPECT--
array(1) {
  [0]=>
  int(2)
}
array(2) {
  [0]=>
  int(2)
  [1]=>
  int(3)
}
