--TEST--
GC 024: GC and objects with non-standard handlers
--INI--
zend.enable_gc=1
--SKIPIF--
<?php if (!extension_loaded("spl")) print "skip SPL extension required"; ?>
--FILE--
<?php

function fn1797898841()
{
    $a = new ArrayObject();
    $a[0] = $a;
    unset($a);
    var_dump(gc_collect_cycles());
    echo "ok\n";
}
fn1797898841();
--EXPECT--
int(2)
ok
