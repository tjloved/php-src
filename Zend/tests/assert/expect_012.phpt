--TEST--
test enable/disable assertions at runtime
--INI--
zend.assertions=1
assert.exception=1
--FILE--
<?php

function fn615077374()
{
    var_dump((int) ini_get("zend.assertions"));
    ini_set("zend.assertions", 0);
    var_dump((int) ini_get("zend.assertions"));
    assert(false);
    ini_set("zend.assertions", 1);
    var_dump((int) ini_get("zend.assertions"));
    assert(true);
    var_dump(true);
}
fn615077374();
--EXPECT--
int(1)
int(0)
int(1)
bool(true)
