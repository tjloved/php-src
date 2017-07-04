--TEST--
jump 01: goto backward
--FILE--
<?php

function fn1831788174()
{
    $n = 1;
    L1:
    echo "{$n}: ok\n";
    $n++;
    if ($n <= 3) {
        goto L1;
    }
}
fn1831788174();
--EXPECT--
1: ok
2: ok
3: ok
