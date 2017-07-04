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
function fn1676200624()
{
    test();
}
fn1676200624();
--EXPECT--
0
1
2
3
4
