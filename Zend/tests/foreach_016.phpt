--TEST--
array_unshift() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn1122390729()
{
    $a = [1, 2, 3];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 2) {
            array_unshift($a, 0, 0, 0, 0, 0, 0, 0, 0);
        }
    }
    var_dump(count($a));
}
fn1122390729();
--EXPECT--
1
2
3
int(11)
