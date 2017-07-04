--TEST--
Tmpvar 003
--FILE--
<?php

function test()
{
    $l = 5;
    for ($i = 0; $i < $l; $i++) {
        echo "{$i}\n";
    }
}
function fn928633026()
{
    test();
}
fn928633026();
--EXPECT--
0
1
2
3
4
