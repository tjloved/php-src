--TEST--
test failing assertion when disabled (with return value)
--INI--
zend.assertions=0
assert.exception=1
--FILE--
<?php

function fn245861088()
{
    var_dump(assert(false));
}
fn245861088();
--EXPECT--
bool(true)