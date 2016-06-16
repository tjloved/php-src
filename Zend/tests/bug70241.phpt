--TEST--
Bug #70241 (Skipped assertions affect Generator returns)
--INI--
zend.assertions=-1
--FILE--
<?php

function foo()
{
    assert((yield 1));
    return null;
}
function fn1308027860()
{
    var_dump(foo() instanceof Generator);
}
fn1308027860();
--EXPECT--
bool(true)
