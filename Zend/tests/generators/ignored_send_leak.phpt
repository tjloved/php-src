--TEST--
Ignoring a sent value shouldn't leak memory
--FILE--
<?php

function gen()
{
    yield;
}
function fn1579903260()
{
    $gen = gen();
    $gen->send(NULL);
    echo "DONE";
}
fn1579903260();
--EXPECT--
DONE
