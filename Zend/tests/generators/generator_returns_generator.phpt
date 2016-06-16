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
function fn815027592()
{
    $generator = gen();
    var_dump($generator instanceof Generator);
}
fn815027592();
--EXPECT--
bool(true)
