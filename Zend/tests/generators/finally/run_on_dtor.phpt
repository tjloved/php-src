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
function fn597315711()
{
    $gen = gen();
    $gen->rewind();
    set_error_handler(function () use($gen) {
    });
}
fn597315711();
--EXPECT--
array(0) {
}
