--TEST--
Identical comparison of array with references
--FILE--
<?php

function fn1578332901()
{
    $foo = 42;
    $array1 = [&$foo];
    $array2 = [$foo];
    var_dump($array1 === $array2);
}
fn1578332901();
--EXPECT--
bool(true)
