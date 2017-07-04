--TEST--
Bug #63982 (isset() inconsistently produces a fatal error on protected property)
--FILE--
<?php

class Test
{
    protected $protectedProperty;
}
function fn1931988890()
{
    $test = new Test();
    var_dump(isset($test->protectedProperty));
    var_dump(isset($test->protectedProperty->foo));
}
fn1931988890();
--EXPECTF--
bool(false)
bool(false)
