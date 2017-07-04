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
function fn1844696694()
{
    try {
        foo();
    } catch (Error $e) {
        echo $e->getMessage(), " in ", $e->getFile(), " on line ", $e->getLine();
    }
}
fn1844696694();
--EXPECTF--
Return value of foo() must be an instance of stdClass, array returned in %s on line %d
