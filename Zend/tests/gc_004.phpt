--TEST--
GC 004: Simple array cycle
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn23519305()
{
    $a = array();
    $a[] =& $a;
    var_dump($a);
    unset($a);
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn23519305();
--EXPECT--
array(1) {
  [0]=>
  &array(1) {
    [0]=>
    *RECURSION*
  }
}
int(1)
ok
