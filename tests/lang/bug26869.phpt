--TEST--
Bug #26869 (constant as the key of static array)
--FILE--
<?php

function fn1835456366()
{
    define("A", "1");
    static $a = array(A => 1);
    var_dump($a);
    var_dump(isset($a[A]));
}
fn1835456366();
--EXPECT--
array(1) {
  [1]=>
  int(1)
}
bool(true)
