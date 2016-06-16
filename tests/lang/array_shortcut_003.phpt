--TEST--
Testing array shortcut and bracket operator
--FILE--
<?php

function fn1997331907()
{
    $a = [1, 2, 3, 4, 5];
    print_r([$a[1], $a[3]]);
}
fn1997331907();
--EXPECT--
Array
(
    [0] => 2
    [1] => 4
)
