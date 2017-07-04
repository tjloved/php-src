--TEST--
A generator function returns a Generator object
--FILE--
<?php

function gen()
{
    // execution is suspended here, so the following never gets run:
    echo "Foo";
    // trigger a generator
    yield;
}
function fn1588958827()
{
    $generator = gen();
    var_dump($generator instanceof Generator);
}
fn1588958827();
--EXPECT--
bool(true)
