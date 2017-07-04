--TEST--
Smart branches 1
--FILE--
<?php

function test(int $a, int $b)
{
    $result = $a < $b;
    $x = 1;
    if ($result) {
        return $result;
    }
}
function fn197095862()
{
    var_dump(test(1, 2));
}
fn197095862();
--EXPECT--
bool(true)
