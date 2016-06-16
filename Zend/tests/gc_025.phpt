--TEST--
GC 025: Automatic GC on request shutdown
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn560127951()
{
    $a = array(array());
    $a[0][0] =& $a[0];
    unset($a);
    echo "ok\n";
}
fn560127951();
--EXPECT--
ok
