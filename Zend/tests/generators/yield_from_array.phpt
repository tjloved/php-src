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
function fn763498194()
{
    foreach (gen() as $v) {
        var_dump($v);
    }
}
fn763498194();
--EXPECT--
int(0)
int(1)
int(2)
