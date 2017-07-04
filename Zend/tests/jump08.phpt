--TEST--
jump 08: goto into loop (forward)
--FILE--
<?php

function fn502559602()
{
    goto L1;
    while (0) {
        L1:
        echo "bug\n";
    }
}
fn502559602();
--EXPECTF--
Fatal error: 'goto' into loop or switch statement is disallowed in %sjump08.php on line %d
