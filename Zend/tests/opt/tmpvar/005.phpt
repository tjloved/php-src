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
function fn244679717()
{
    var_dump(test(1, 2, false));
}
fn244679717();
--EXPECT--
NULL
