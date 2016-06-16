--TEST--
GC 013: Too many cycles in one array
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn1496818554()
{
    $a = array();
    for ($i = 0; $i < 10001; $i++) {
        $a[$i] =& $a;
    }
    $a[] = "xxx";
    unset($a);
    var_dump(gc_collect_cycles() > 0);
    echo "ok\n";
}
fn1496818554();
--EXPECT--
bool(true)
ok
