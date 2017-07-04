--TEST--
Bug #67111: Memory leak when using "continue 2" inside two foreach loops
--FILE--
<?php

function fn1508061896()
{
    $array1 = [1, 2, 3];
    $array2 = [1, 2, 3];
    foreach ($array1 as $x) {
        foreach ($array2 as $y) {
            echo "{$x}.{$y}\n";
            continue 2;
        }
    }
}
fn1508061896();
--EXPECT--
1.1
2.1
3.1
