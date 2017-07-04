--TEST--
jump 03: goto inside control structures
--FILE--
<?php

function fn1025842312()
{
    do {
        if (1) {
            echo "1: ok\n";
            goto L1;
        } else {
            echo "bug\n";
            L1:
            echo "2: ok\n";
        }
    } while (0);
}
fn1025842312();
--EXPECT--
1: ok
2: ok
