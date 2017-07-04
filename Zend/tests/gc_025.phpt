--TEST--
GC 025: Automatic GC on request shutdown
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn71155069()
{
    $a = array(array());
    $a[0][0] =& $a[0];
    unset($a);
    echo "ok\n";
}
fn71155069();
--EXPECT--
ok
