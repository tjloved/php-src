--TEST--
Bug #65969 (Chain assignment with T_LIST failure)
--FILE--
<?php

function fn55961375()
{
    $obj = new stdClass();
    list($a, $b) = $obj->prop = [1, 2];
    var_dump($a, $b);
}
fn55961375();
--EXPECT--
int(1)
int(2)
