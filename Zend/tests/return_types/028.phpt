--TEST--
Memory leak when returning TMP/VAR with wrong return type
--FILE--
<?php

function foo() : stdClass
{
    $a = new stdClass();
    $b = [];
    return [$a, $b];
}
function fn639557443()
{
    try {
        foo();
    } catch (Error $e) {
        echo $e->getMessage(), " in ", $e->getFile(), " on line ", $e->getLine();
    }
}
fn639557443();
--EXPECTF--
Return value of foo() must be an instance of stdClass, array returned in %s on line %d
