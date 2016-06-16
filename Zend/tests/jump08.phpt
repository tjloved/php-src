--TEST--
jump 08: goto into loop (forward)
--FILE--
<?php

function fn2002675928()
{
    goto L1;
    while (0) {
        L1:
        echo "bug\n";
    }
}
fn2002675928();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump08.php on line %d
