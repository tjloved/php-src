--TEST--
Try finally (segfault with empty break)
--FILE--
<?php

function foo()
{
    try {
        break;
    } finally {
    }
}
function fn2017949891()
{
    foo();
}
fn2017949891();
--EXPECTF--
Fatal error: 'break' not in the 'loop' or 'switch' context in %stry_finally_011.php on line %d
