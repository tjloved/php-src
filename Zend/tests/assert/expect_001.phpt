--TEST--
test passing assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn1007154452()
{
    assert(true);
    var_dump(true);
}
fn1007154452();
--EXPECTF--
bool(true)
