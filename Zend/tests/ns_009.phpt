--TEST--
009: __NAMESPACE__ constant and runtime names (php name)
--FILE--
<?php

class foo
{
}
function fn523497276()
{
    $x = __NAMESPACE__ . "\\foo";
    echo get_class(new $x()), "\n";
}
fn523497276();
--EXPECT--
foo
