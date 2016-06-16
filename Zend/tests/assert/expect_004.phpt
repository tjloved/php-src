--TEST--
test providing reason (fail)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn1925973689()
{
    try {
        assert(false, "I require this to succeed");
    } catch (AssertionError $ex) {
        var_dump($ex->getMessage());
    }
}
fn1925973689();
--EXPECT--
string(25) "I require this to succeed"
