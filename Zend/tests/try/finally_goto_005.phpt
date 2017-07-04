--TEST--
There must be a difference between label: try { ... } and try { label: ... }
--FILE--
<?php

function fn1205596105()
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
fn1205596105();
--EXPECT--
success
