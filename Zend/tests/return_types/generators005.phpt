--TEST--
Return type covariance works with generators

--FILE--
<?php

interface Collection extends IteratorAggregate
{
    function getIterator() : Iterator;
}
class SomeCollection implements Collection
{
    function getIterator() : Iterator
    {
        foreach ($this->data as $key => $value) {
            (yield $key => $value);
        }
    }
}
function fn1999321388()
{
    $some = new SomeCollection();
    var_dump($some->getIterator());
}
fn1999321388();
--EXPECTF--
object(Generator)#%d (%d) {
}
