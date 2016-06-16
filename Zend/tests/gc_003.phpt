--TEST--
GC 003: gc_enabled() and ini_set()
--FILE--
<?php

function fn494271879()
{
    ini_set('zend.enable_gc', '0');
    var_dump(gc_enabled());
    echo ini_get('zend.enable_gc') . "\n";
    ini_set('zend.enable_gc', '1');
    var_dump(gc_enabled());
    echo ini_get('zend.enable_gc') . "\n";
}
fn494271879();
--EXPECT--
bool(false)
0
bool(true)
1
