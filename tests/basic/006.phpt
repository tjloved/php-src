--TEST--
Add 3 variables together and print result
--FILE--
<?php

function fn1767369771()
{
    $a = 1;
    $b = 2;
    $c = 3;
    $d = $a + $b + $c;
    echo $d;
}
fn1767369771();
--EXPECT--
6
