--TEST--
array_pop() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn761927213()
{
    $a = [1, 2, 3];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 2) {
            array_pop($a);
        }
    }
}
fn761927213();
--EXPECT--
1
2
