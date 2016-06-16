--TEST--
Multiply 3 variables and print result
--FILE--
<?php

function fn2123362711()
{
    $a = 2;
    $b = 4;
    $c = 8;
    $d = $a * $b * $c;
    echo $d;
}
fn2123362711();
--EXPECT--
64
