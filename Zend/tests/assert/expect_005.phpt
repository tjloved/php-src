--TEST--
test providing reason (pass)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn831947121()
{
    try {
        /* by passing we test there are no leaks upon success */
        assert(true, "I require this to succeed");
    } catch (AssertionError $ex) {
        var_dump($ex->getMessage());
    }
    var_dump(true);
}
fn831947121();
--EXPECT--
bool(true)
