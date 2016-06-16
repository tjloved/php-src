--TEST--
Closure 006: Nested lambdas
--FILE--
<?php

function fn2084248664()
{
    $getClosure = function ($v) {
        return function () use($v) {
            echo "Hello World: {$v}!\n";
        };
    };
    $closure = $getClosure(2);
    $closure();
    echo "Done\n";
}
fn2084248664();
--EXPECT--
Hello World: 2!
Done
