--TEST--
Bug #65035: yield / exit segfault
--FILE--
<?php

function gen()
{
    fn();
    yield;
}
function fn()
{
    exit('Done');
}
function fn1768464680()
{
    $gen = gen();
    $gen->current();
}
fn1768464680();
--EXPECT--
Done
