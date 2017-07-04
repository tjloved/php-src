--TEST--
jump 09: goto into switch (backward)
--FILE--
<?php

function fn1248939352()
{
    switch (0) {
        case 1:
            L1:
            echo "bug\n";
            break;
    }
    goto L1;
}
fn1248939352();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump09.php on line %d
