--TEST--
Constant propagation 002
--FILE--
<?php

function test($b)
{
    $a = 1;
    if ($b) {
        $a = 2;
    } else {
        $a = 2;
    }
    return $a;
    // 2

}
function fn1094527775()
{
    var_dump(test(true));
    var_dump(test(false));
}
fn1094527775();
--EXPECT--
int(2)
int(2)
