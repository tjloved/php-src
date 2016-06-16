--TEST--
009: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

class foo
{
}
function fn1579897565()
{
    $x = __NAMESPACE__ . "\\foo";
    echo get_class(new $x()), "\n";
}
fn1579897565();
--EXPECT--
foo
