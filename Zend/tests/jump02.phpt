--TEST--
jump 02: goto forward
--FILE--
<?php

function fn732401736()
{
    $n = 1;
    L1:
    if ($n > 3) {
        goto L2;
    }
    echo "{$n}: ok\n";
    $n++;
    goto L1;
    L2:
}
fn732401736();
--EXPECT--
1: ok
2: ok
3: ok
