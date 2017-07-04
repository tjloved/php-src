--TEST--
Bug #34137 (assigning array element by reference causes binary mess)
--FILE--
<?php

function fn2003491228()
{
    $arr1 = array('a1' => array('alfa' => 'ok'));
    $arr1 =& $arr1['a1'];
    echo '-' . $arr1['alfa'] . "-\n";
}
fn2003491228();
--EXPECT--
-ok-
