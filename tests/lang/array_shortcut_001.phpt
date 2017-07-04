--TEST--
Square bracket array shortcut test
--FILE--
<?php

function fn2025807091()
{
    print_r([1, 2, 3]);
}
fn2025807091();
--EXPECT--
Array
(
    [0] => 1
    [1] => 2
    [2] => 3
)
