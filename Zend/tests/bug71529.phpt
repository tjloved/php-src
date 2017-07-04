--TEST--
Bug #71529: Variable references on array elements don't work when using count
--FILE--
<?php

function out($what)
{
    var_dump($what);
    return $what;
}
function fn944530310()
{
    $a = [1];
    $a[] =& $a[out(count($a) - 1)];
    var_dump($a);
}
fn944530310();
--EXPECT--
int(0)
array(2) {
  [0]=>
  &int(1)
  [1]=>
  &int(1)
}
