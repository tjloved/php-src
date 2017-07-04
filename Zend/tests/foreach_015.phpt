--TEST--
array_shift() function precerve foreach by reference iterator pointer
--FILE--
<?php

function fn1452464913()
{
    $a = [1, 2, 3, 4];
    foreach ($a as &$v) {
        echo "{$v}\n";
        array_shift($a);
    }
    var_dump($a);
}
fn1452464913();
--EXPECT--
1
2
3
4
array(0) {
}