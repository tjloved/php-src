--TEST--
jump 07: goto into loop (backward)
--FILE--
<?php

function fn1618576916()
{
    while (0) {
        L1:
        echo "bug\n";
    }
    goto L1;
}
fn1618576916();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump07.php on line %d
