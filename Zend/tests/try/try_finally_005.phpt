--TEST--
Try finally (with long goto)
--FILE--
<?php

function foo()
{
    try {
    } finally {
        goto label;
    }
    label:
    return 1;
}
function fn1594882977()
{
    foo();
}
fn1594882977();
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_005.php on line %d
