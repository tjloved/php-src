--TEST--
Subtract 3 variables and print result
--FILE--
<?php

function fn1991068884()
{
    $a = 27;
    $b = 7;
    $c = 10;
    $d = $a - $b - $c;
    echo $d;
}
fn1991068884();
--EXPECT--
10
