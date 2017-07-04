--TEST--
Constant propagation 003
--FILE--
<?php

function test($x)
{
    $i = 0;
    $j = 1;
    while ($x) {
        $j = $i;
        // 0
        $i += 1;
        $i = $j;
        // 0
    }
    return $i;
    // 0
}
function fn1067678788()
{
    var_dump(test(false));
}
fn1067678788();
--EXPECT--
int(0)
