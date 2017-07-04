--TEST--
test looping assert (pass)
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn160769862()
{
    for ($i = 0; $i < 100000; $i++) {
        assert($i < 100000, "The universe should make sense");
    }
    var_dump(true);
}
fn160769862();
--EXPECT--
bool(true)
