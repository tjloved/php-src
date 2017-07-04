--TEST--
test failing assertion when disabled (with return value)
--INI--
zend.assertions=0
assert.exception=1
--FILE--
<?php

function fn1179323496()
{
    var_dump(assert(false));
}
fn1179323496();
--EXPECT--
bool(true)