--TEST--
Tmpvar 002
--FILE--
<?php

function test()
{
    $l = 5;
    for ($i = 0; $i < $l; $i += 1) {
        echo "{$i}\n";
    }
}
function fn1511327540()
{
    test();
}
fn1511327540();
--EXPECT--
0
1
2
3
4
