--TEST--
GC 027: GC and properties of internal classes
--INI--
zend.enable_gc=1
--FILE--
<?php

function fn2146076285()
{
    try {
        throw new Exception();
    } catch (Exception $e) {
        gc_collect_cycles();
    }
    echo "ok\n";
}
fn2146076285();
--EXPECT--
ok
