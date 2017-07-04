--TEST--
array_push() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn721366971()
{
    $a = [1, 2, 3];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 3) {
            array_push($a, 4);
        }
    }
}
fn721366971();
--EXPECT--
1
2
3
4
