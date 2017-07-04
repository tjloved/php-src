--TEST--
Multiply 3 variables and print result
--FILE--
<?php

function fn522443344()
{
    $a = 2;
    $b = 4;
    $c = 8;
    $d = $a * $b * $c;
    echo $d;
}
fn522443344();
--EXPECT--
64
