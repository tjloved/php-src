--TEST--
GC 031: gc_collect_roots() with GC turned off.
--INI--
zend.enable_gc=0
--FILE--
<?php

function fn1001673157()
{
    gc_collect_cycles();
    echo "DONE\n";
}
fn1001673157();
--EXPECTF--
DONE
