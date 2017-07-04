--TEST--
jump 10: goto into switch (forward)
--FILE--
<?php

function fn1499035363()
{
    goto L1;
    switch (0) {
        case 1:
            L1:
            echo "bug\n";
            break;
    }
}
fn1499035363();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump10.php on line %d
