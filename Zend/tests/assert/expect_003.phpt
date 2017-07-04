--TEST--
test catching failed assertion
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn981638865()
{
    try {
        assert(false);
    } catch (AssertionError $ex) {
        var_dump($ex->getMessage());
    }
}
fn981638865();
--EXPECT--
string(13) "assert(false)"
