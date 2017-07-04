--TEST--
foreach() with references
--FILE--
<?php

function fn187803587()
{
    $arr = array(1 => "one", 2 => "two", 3 => "three");
    foreach ($arr as $key => $val) {
        $val = $key;
    }
    print_r($arr);
    foreach ($arr as $key => &$val) {
        $val = $key;
    }
    print_r($arr);
}
fn187803587();
--EXPECT--
Array
(
    [1] => one
    [2] => two
    [3] => three
)
Array
(
    [1] => 1
    [2] => 2
    [3] => 3
)
