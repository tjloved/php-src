--TEST--
Divide 3 variables and print result
--FILE--
<?php

function fn373200979()
{
    $a = 27;
    $b = 3;
    $c = 3;
    $d = $a / $b / $c;
    echo $d;
}
fn373200979();
--EXPECT--
3
