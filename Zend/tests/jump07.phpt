--TEST--
jump 07: goto into loop (backward)
--FILE--
<?php

function fn1646929071()
{
    while (0) {
        L1:
        echo "bug\n";
    }
    goto L1;
}
fn1646929071();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump07.php on line %d
