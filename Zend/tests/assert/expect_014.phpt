--TEST--
test failing assertion when disabled
--INI--
zend.assertions=0
assert.exception=1
--FILE--
<?php

function fn1472942591()
{
    assert(false);
    var_dump(true);
}
fn1472942591();
--EXPECT--
bool(true)