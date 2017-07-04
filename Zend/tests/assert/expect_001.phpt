--TEST--
test passing assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn373792171()
{
    assert(true);
    var_dump(true);
}
fn373792171();
--EXPECTF--
bool(true)
