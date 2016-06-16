--TEST--
Smart branches 2
--FILE--
<?php

function test(int $a, int $b, bool $return)
{
    $result = $a < $b;
    if ($return) {
        return $result;
    }
}
function fn1205245392()
{
    var_dump(test(1, 2, false));
}
fn1205245392();
--EXPECT--
NULL
