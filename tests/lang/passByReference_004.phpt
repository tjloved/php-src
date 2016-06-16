--TEST--
passing the return value from a function by reference
--FILE--
<?php

function foo(&$ref)
{
    var_dump($ref);
}
function bar($value)
{
    return $value;
}
function fn1691051779()
{
    foo(bar(5));
}
fn1691051779();
--EXPECTF--
Notice: Only variables should be passed by reference in %s on line %d
int(5)
