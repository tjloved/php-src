--TEST--
Generators must return a valid variable with return type specified
--FILE--
<?php

function fn1303992970()
{
    $gen = (function () : Generator {
        1 + ($a = 1);
        // force a temporary
        return true;
        yield;
    })();
    var_dump($gen->valid());
    var_dump($gen->getReturn());
}
fn1303992970();
--EXPECT--
bool(false)
bool(true)
