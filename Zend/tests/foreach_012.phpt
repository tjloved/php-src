--TEST--
array_walk() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn972482784()
{
    $a = [1, 2, 3, 4, 5];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 3) {
            array_walk($a, function (&$x) {
                $x += 10;
            });
        }
    }
}
fn972482784();
--EXPECT--
1
2
3
14
15