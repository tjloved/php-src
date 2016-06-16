--TEST--
There must be a difference between label: try { ... } and try { label: ... }
--FILE--
<?php

function fn799950547()
{
    label:
    try {
        goto label;
    } finally {
        print "success";
        return;
        // don't loop
    
    }
}
fn799950547();
--EXPECT--
success
