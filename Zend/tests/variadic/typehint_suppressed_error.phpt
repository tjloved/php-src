--TEST--
Error suppression for types on variadic arguments works
--FILE--
<?php

function test(array ...$args)
{
    var_dump($args);
}
function fn1741461162()
{
    try {
        test([0], [1], 2);
    } catch (Error $e) {
        var_dump($e->getMessage());
    }
}
fn1741461162();
--EXPECTF--
string(%d) "Argument 3 passed to test() must be of the type array, integer given, called in %s on line %d"
