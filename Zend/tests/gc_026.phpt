--TEST--
GC 026: Automatic GC on request shutdown (GC enabled at run-time)
--INI--
zend.enable_gc=0
--FILE--
<?php

function fn406495411()
{
    gc_enable();
    $a = array(array());
    $a[0][0] =& $a[0];
    unset($a);
    echo "ok\n";
}
fn406495411();
--EXPECT--
ok
