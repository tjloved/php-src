--TEST--
Bug #67111: Memory leak when using "continue 2" inside two foreach loops
--FILE--
<?php

function fn2099476340()
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
fn2099476340();
--EXPECT--
1.1
2.1
3.1
