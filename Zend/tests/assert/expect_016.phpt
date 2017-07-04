--TEST--
test enable/disable assertions at runtime (assertions not completely disabled)
--INI--
zend.assertions=0
assert.exception=0
--FILE--
<?php

function fn693301910()
{
    ini_set("zend.assertions", 0);
    var_dump(assert(false));
    var_dump(assert(true));
    ini_set("zend.assertions", 1);
    var_dump(assert(false));
    var_dump(assert(true));
    ini_set("zend.assertions", -1);
}
fn693301910();
--EXPECTF--
bool(true)
bool(true)

Warning: assert(): assert(false) failed in %sexpect_016.php on line %d
bool(false)
bool(true)

Warning: zend.assertions may be completely enabled or disabled only in php.ini in %sexpect_016.php on line %d
