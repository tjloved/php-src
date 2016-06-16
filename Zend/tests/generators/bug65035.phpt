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
function fn1805882749()
{
    $gen = gen();
    $gen->current();
}
fn1805882749();
--EXPECT--
Done
