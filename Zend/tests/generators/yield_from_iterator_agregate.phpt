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
function fn261299869()
{
    (function () {
        yield from new foo();
    })()->next();
}
fn261299869();
--EXPECT--
int(1)
