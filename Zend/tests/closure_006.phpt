--TEST--
Closure 006: Nested lambdas
--FILE--
<?php

function fn1995700511()
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
fn1995700511();
--EXPECT--
Hello World: 2!
Done
