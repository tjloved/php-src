--TEST--
test enable/disable assertions at runtime (assertions completely disabled)
--INI--
zend.assertions=-1
assert.exception=0
--FILE--
<?php

function fn2011882258()
{
    var_dump(assert(false));
    var_dump(assert(true));
    ini_set("zend.assertions", 0);
    ini_set("zend.assertions", 1);
}
fn2011882258();
--EXPECTF--
bool(true)
bool(true)

Warning: zend.assertions may be completely enabled or disabled only in php.ini in %sexpect_017.php on line %d

Warning: zend.assertions may be completely enabled or disabled only in php.ini in %sexpect_017.php on line %d
