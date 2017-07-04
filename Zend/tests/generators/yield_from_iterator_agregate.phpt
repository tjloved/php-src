--TEST--
yield from with an IteratorAggregate
--FILE--
<?php

class foo implements \IteratorAggregate
{
    public $prop = 1;
    function getIterator()
    {
        var_dump($this->prop);
        yield;
    }
}
function fn1077817414()
{
    (function () {
        yield from new foo();
    })()->next();
}
fn1077817414();
--EXPECT--
int(1)
