--TEST--
yielding values from an array
--FILE--
<?php

function from()
{
    (yield 0);
    yield from [];
    // must not yield anything
    yield from [1, 2];
}
function gen()
{
    yield from from();
}
function fn478204509()
{
    foreach (gen() as $v) {
        var_dump($v);
    }
}
fn478204509();
--EXPECT--
int(0)
int(1)
int(2)
