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
function fn833081195()
{
    var_dump(test(false));
}
fn833081195();
--EXPECT--
int(0)
