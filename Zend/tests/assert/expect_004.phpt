--TEST--
test providing reason (fail)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn219221685()
{
    try {
        assert(false, "I require this to succeed");
    } catch (AssertionError $ex) {
        var_dump($ex->getMessage());
    }
}
fn219221685();
--EXPECT--
string(25) "I require this to succeed"
