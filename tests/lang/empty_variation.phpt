--TEST--
empty() on array elements
--FILE--
<?php

function fn757023067()
{
    $a = array('0', 'empty' => '0');
    var_dump(empty($a['empty']));
    var_dump(empty($a[0]));
    $b = '0';
    var_dump(empty($b));
}
fn757023067();
--EXPECT--
bool(true)
bool(true)
bool(true)
