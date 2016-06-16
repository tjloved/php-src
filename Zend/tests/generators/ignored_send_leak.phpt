--TEST--
Ignoring a sent value shouldn't leak memory
--FILE--
<?php

function gen()
{
    yield;
}
function fn1453128553()
{
    $gen = gen();
    $gen->send(NULL);
    echo "DONE";
}
fn1453128553();
--EXPECT--
DONE
