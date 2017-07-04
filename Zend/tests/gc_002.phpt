--TEST--
GC 002: gc_enable()/gc_diable() and ini_get()
--FILE--
<?php

function fn720579295()
{
    gc_disable();
    var_dump(gc_enabled());
    echo ini_get('zend.enable_gc') . "\n";
    gc_enable();
    var_dump(gc_enabled());
    echo ini_get('zend.enable_gc') . "\n";
}
fn720579295();
--EXPECT--
bool(false)
0
bool(true)
1
