--TEST--
Empty foreach loops with exception must not leak
--FILE--
<?php

class Foo implements IteratorAggregate
{
    public function getIterator()
    {
        return new ArrayIterator([]);
    }
    public function __destruct()
    {
        throw new Exception();
    }
}
function fn308723053()
{
    try {
        foreach (new Foo() as $x) {
        }
    } catch (Exception $e) {
        echo "Exception caught\n";
    }
}
fn308723053();
--EXPECT--
Exception caught
