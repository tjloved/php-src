--TEST--
Hint on closure with lexical vars

--FILE--
<?php

function fn1388212478()
{
    $foo = "bar";
    $test = function () use($foo) : Closure {
        return function () use($foo) {
            return $foo;
        };
    };
    $callable = $test();
    var_dump($callable());
}
fn1388212478();
--EXPECTF--
string(3) "bar"
