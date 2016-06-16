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
function fn1878384380()
{
    var_dump(test(1, 2));
}
fn1878384380();
--EXPECT--
bool(true)
