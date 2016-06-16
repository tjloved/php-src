--TEST--
Divide 3 variables and print result
--FILE--
<?php

function fn1822848436()
{
    $a = 27;
    $b = 3;
    $c = 3;
    $d = $a / $b / $c;
    echo $d;
}
fn1822848436();
--EXPECT--
3
