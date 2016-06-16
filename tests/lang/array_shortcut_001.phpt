--TEST--
Square bracket array shortcut test
--FILE--
<?php

function fn1602723254()
{
    print_r([1, 2, 3]);
}
fn1602723254();
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
