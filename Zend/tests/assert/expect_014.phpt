--TEST--
test failing assertion when disabled
--INI--
zend.assertions=0
assert.exception=1
--FILE--
<?php

function fn1442359204()
{
    assert(false);
    var_dump(true);
}
fn1442359204();
--EXPECT--
bool(true)