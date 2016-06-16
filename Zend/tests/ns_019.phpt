--TEST--
019: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

function foo()
{
    return __FUNCTION__;
}
function fn1040661553()
{
    $x = __NAMESPACE__ . "\\foo";
    echo $x(), "\n";
}
fn1040661553();
--EXPECT--
foo
