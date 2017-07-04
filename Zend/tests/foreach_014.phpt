--TEST--
array_pop() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn1679730590()
{
    $a = [1, 2, 3];
    foreach ($a as &$v) {
        echo "{$v}\n";
        if ($v == 2) {
            array_pop($a);
        }
    }
}
fn1679730590();
--EXPECT--
1
2
