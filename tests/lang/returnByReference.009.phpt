--TEST--
Returning a references returned by another function
--FILE--
<?php

function &returnVarByRef()
{
    $b = 1;
    return $b;
}
function &testReturnVarByRef()
{
    return returnVarByRef();
}
function returnVal()
{
    return 1;
}
function &testReturnValByRef()
{
    return returnVal();
}
function fn837090417()
{
    echo "\n---> 1. Return a variable by reference -> No warning:\n";
    var_dump(testReturnVarByRef());
    echo "\n---> 2. Return a value by reference -> Warning:\n";
    var_dump(testReturnValByRef());
}
fn837090417();
--EXPECTF--
---> 1. Return a variable by reference -> No warning:
int(1)

---> 2. Return a value by reference -> Warning:

Notice: Only variable references should be returned by reference in %s on line %d
int(1)
