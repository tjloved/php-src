--TEST--
Identical comparison of array with references
--FILE--
<?php

function fn138835124()
{
    $foo = 42;
    $array1 = [&$foo];
    $array2 = [$foo];
    var_dump($array1 === $array2);
}
fn138835124();
--EXPECT--
bool(true)
