--TEST--
GC 001: gc_enable()/gc_diable()/gc_enabled()
--FILE--
<?php

function fn1192889609()
{
    gc_disable();
    var_dump(gc_enabled());
    gc_enable();
    var_dump(gc_enabled());
}
fn1192889609();
--EXPECT--
bool(false)
bool(true)
