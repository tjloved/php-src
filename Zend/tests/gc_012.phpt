--TEST--
GC 012: collection of many loops at once
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn880394989()
{
    $a = array();
    for ($i = 0; $i < 1000; $i++) {
        $a[$i] = array(array());
        $a[$i][0] =& $a[$i];
    }
    var_dump(gc_collect_cycles());
    unset($a);
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn880394989();
--EXPECT--
int(0)
int(1000)
ok
