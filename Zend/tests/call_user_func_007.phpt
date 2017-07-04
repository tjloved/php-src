--TEST--
call_user_func() should not use FUNC_ARG fetches
--FILE--
<?php

function foo(&$ref)
{
    $ref = 24;
}
function fn1618120712()
{
    $a = [];
    call_user_func('foo', $a[0][0]);
    var_dump($a);
}
fn1618120712();
--EXPECTF--
Notice: Undefined offset: 0 in %s on line %d

Warning: Parameter 1 to foo() expected to be a reference, value given in %s on line %d
array(0) {
}
