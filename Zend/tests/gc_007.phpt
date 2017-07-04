--TEST--
GC 007: Unreferenced array cycle
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn570610072()
{
    $a = array(array());
    $a[0][0] =& $a[0];
    var_dump($a[0]);
    var_dump(gc_collect_cycles());
    unset($a);
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn570610072();
--EXPECT--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}
int(0)
int(1)
ok
