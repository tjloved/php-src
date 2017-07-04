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
function fn383790355()
{
    $some = new SomeCollection();
    var_dump($some->getIterator());
}
fn383790355();
--EXPECTF--
object(Generator)#%d (%d) {
}
