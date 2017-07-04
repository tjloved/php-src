--TEST--
Bug #69376 (Wrong ref counting)
--FILE--
<?php

function fn453260579()
{
    $array = array();
    $array[] =& $array;
    $a = $array;
    unset($array);
    $b = $a;
    $b[0] = 123;
    print_r($a);
    print_r($b);
}
fn453260579();
--EXPECT--
Array
(
    [0] => 123
)
Array
(
    [0] => 123
)
