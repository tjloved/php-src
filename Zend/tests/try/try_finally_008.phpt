--TEST--
Try finally (with break in do...while)
--FILE--
<?php

function foo()
{
    do {
        try {
            try {
            } finally {
                break;
            }
        } catch (Exception $e) {
        } finally {
        }
    } while (0);
}
function fn230821977()
{
    foo();
}
fn230821977();
--EXPECTF--
Fatal error: jump out of a finally block is disallowed in %stry_finally_008.php on line %d
