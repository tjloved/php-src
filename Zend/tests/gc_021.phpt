--TEST--
GC 021: GC detach reference with assign by reference
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn243780628()
{
    $a = array();
    $a[0] =& $a;
    $a[1] = array();
    $a[1][0] =& $a[1];
    $b = 1;
    $a =& $b;
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn243780628();
--EXPECT--
int(2)
ok
