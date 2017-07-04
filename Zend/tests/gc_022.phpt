--TEST--
GC 022: GC detach reference in executor's PZVAL_UNLOCK()
--INI--
error_reporting=0
zend.enable_gc=1
--FILE--
<?php

function fn907837854()
{
    $a = array(array());
    $a[0][0] =& $a[0];
    $s = array(1) + unserialize(serialize($a[0]));
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn907837854();
--EXPECT--
int(1)
ok
