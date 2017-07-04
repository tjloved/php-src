--TEST--
finally is run on object dtor, not free
--FILE--
<?php

function gen()
{
    try {
        yield;
    } finally {
        var_dump($_GET);
    }
}
function fn2097533868()
{
    $gen = gen();
    $gen->rewind();
    set_error_handler(function () use($gen) {
    });
}
fn2097533868();
--EXPECT--
array(0) {
}
