--TEST--
array_push() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn1194421267()
{
    $a = [1, 2, 3];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 3) {
            array_push($a, 4);
        }
    }
}
fn1194421267();
--EXPECT--
1
2
3
4
