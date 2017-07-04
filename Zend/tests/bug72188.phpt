--TEST--
Bug #72188 (Nested try/finally blocks losing return value)
--FILE--
<?php

function test()
{
    try {
        return 5;
    } finally {
        try {
            echo 1;
        } finally {
            echo 2;
        }
    }
}
function fn1036865460()
{
    $a = test();
    if ($a !== 5) {
        echo "FAILED: expected 5, received ", var_export($a), PHP_EOL;
    } else {
        echo "Passed", PHP_EOL;
    }
}
fn1036865460();
--EXPECT--
12Passed
