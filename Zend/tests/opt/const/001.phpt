--TEST--
Constant propation 001
--FILE--
<?php

function test()
{
    $a = 1;
    $b = 2;
    $c = $a + $b;
    // 3
    $d = $a + $c;
    // 4
    $e = $c + $d;
    // 7
    return $e;
    // 7
}
function fn375013481()
{
    var_dump(test());
}
fn375013481();
--EXPECT--
int(7)
