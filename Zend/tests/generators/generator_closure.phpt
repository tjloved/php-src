--TEST--
Closures can be generators
--FILE--
<?php

function fn2107698521()
{
    $genFactory = function () {
        (yield 1);
        (yield 2);
        (yield 3);
    };
    foreach ($genFactory() as $value) {
        var_dump($value);
    }
}
fn2107698521();
--EXPECT--
int(1)
int(2)
int(3)
