--TEST--
Tmpvar 001
--FILE--
<?php

function test()
{
    $l = 5;
    for ($i = 0; $i < $l; $i = $i + 1) {
        echo "{$i}\n";
    }
}
function fn859568228()
{
    test();
}
fn859568228();
--EXPECT--
0
1
2
3
4
