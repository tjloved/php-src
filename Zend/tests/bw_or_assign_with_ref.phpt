--TEST--
Bitwise or assign with referenced value
--FILE--
<?php

function fn501525072()
{
    $num1 = 1;
    $num2 = '2';
    $ref =& $num2;
    $num1 |= $num2;
    var_dump($num1);
}
fn501525072();
--EXPECT--
int(3)
