--TEST--
GC 001: gc_enable()/gc_diable()/gc_enabled()
--FILE--
<?php

function fn415152103()
{
    gc_disable();
    var_dump(gc_enabled());
    gc_enable();
    var_dump(gc_enabled());
}
fn415152103();
--EXPECT--
bool(false)
bool(true)
