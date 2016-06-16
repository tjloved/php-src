--TEST--
Closures can be generators
--FILE--
<?php

function fn1847406642()
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
fn1847406642();
--EXPECT--
int(1)
int(2)
int(3)
