--TEST--
GC 026: Automatic GC on request shutdown (GC enabled at run-time)
--INI--
zend.enable_gc=0
--FILE--
<?php

function fn164794453()
{
    gc_enable();
    $a = array(array());
    $a[0][0] =& $a[0];
    unset($a);
    echo "ok\n";
}
fn164794453();
--EXPECT--
ok
