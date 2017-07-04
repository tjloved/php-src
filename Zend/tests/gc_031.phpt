--TEST--
GC 031: gc_collect_roots() with GC turned off.
--INI--
zend.enable_gc=0
--FILE--
<?php

function fn930345494()
{
    gc_collect_cycles();
    echo "DONE\n";
}
fn930345494();
--EXPECTF--
DONE
