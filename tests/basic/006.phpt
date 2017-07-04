--TEST--
Add 3 variables together and print result
--FILE--
<?php

function fn1966547063()
{
    $a = 1;
    $b = 2;
    $c = 3;
    $d = $a + $b + $c;
    echo $d;
}
fn1966547063();
--EXPECT--
6
