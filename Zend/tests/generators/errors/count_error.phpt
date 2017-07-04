--TEST--
Generators can't be counted
--FILE--
<?php

function gen()
{
    yield;
}
function fn1208882745()
{
    $gen = gen();
    try {
        count($gen);
    } catch (Exception $e) {
        echo $e;
    }
}
fn1208882745();
--EXPECTF--
Warning: count(): Parameter must be an array or an object that implements Countable in %s on line %d
