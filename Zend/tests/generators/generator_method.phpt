--TEST--
Methods can be generators
--FILE--
<?php

class Test implements IteratorAggregate
{
    protected $data;
    public function __construct(array $data)
    {
        $this->data = $data;
    }
    public function getIterator()
    {
        foreach ($this->data as $value) {
            (yield $value);
        }
    }
}
function fn1813626285()
{
    $test = new Test(['foo', 'bar', 'baz']);
    foreach ($test as $value) {
        var_dump($value);
    }
}
fn1813626285();
--EXPECT--
string(3) "foo"
string(3) "bar"
string(3) "baz"
