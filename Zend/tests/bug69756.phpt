--TEST--
Fixed bug #69756 (Fatal error: Nesting level too deep - recursive dependency? with ===).
--FILE--
<?php

function fn324182658()
{
    $arr = range(1, 2);
    foreach ($arr as &$item) {
        var_dump($arr === array(1, 2));
    }
}
fn324182658();
--EXPECT--
bool(true)
bool(true)
