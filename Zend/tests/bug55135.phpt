--TEST--
Bug #55135 (Array keys are no longer type casted in unset())
--FILE--
<?php

function fn1554084903()
{
    // This fails.
    $array = array(1 => 2);
    $a = "1";
    unset($array[$a]);
    print_r($array);
    // Those works.
    $array = array(1 => 2);
    $a = 1;
    unset($array[$a]);
    print_r($array);
    $array = array(1 => 2);
    unset($array[1]);
    print_r($array);
    $array = array(1 => 2);
    $a = 1;
    unset($array["1"]);
    print_r($array);
}
fn1554084903();
--EXPECT--
Array
(
)
Array
(
)
Array
(
)
Array
(
)
