--TEST--
019: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

function foo()
{
    return __FUNCTION__;
}
function fn1972527683()
{
    $x = __NAMESPACE__ . "\\foo";
    echo $x(), "\n";
}
fn1972527683();
--EXPECT--
foo
