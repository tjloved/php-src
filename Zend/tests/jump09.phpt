--TEST--
jump 09: goto into switch (backward)
--FILE--
<?php

function fn123235982()
{
    switch (0) {
        case 1:
            L1:
            echo "bug\n";
            break;
    }
    goto L1;
}
fn123235982();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump09.php on line %d
