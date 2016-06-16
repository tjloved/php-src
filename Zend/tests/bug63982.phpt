--TEST--
Bug #63982 (isset() inconsistently produces a fatal error on protected property)
--FILE--
<?php

class Test
{
    protected $protectedProperty;
}
function fn1609938166()
{
    $test = new Test();
    var_dump(isset($test->protectedProperty));
    var_dump(isset($test->protectedProperty->foo));
}
fn1609938166();
--EXPECTF--
bool(false)
bool(false)
