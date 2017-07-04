--TEST--
Bug #49269 (Ternary operator fails on Iterator object when used inside foreach declaration).
--FILE--
<?php

class TestObject implements Iterator
{
    public $n = 0;
    function valid()
    {
        return $this->n < 3;
    }
    function current()
    {
        return $this->n;
    }
    function next()
    {
        $this->n++;
    }
    function key()
    {
    }
    function rewind()
    {
        $this->n = 0;
    }
}
function fn1283986334()
{
    $array_object = new TestObject();
    foreach (true ? $array_object : $array_object as $item) {
        echo "{$item}\n";
    }
}
fn1283986334();
--EXPECT--
0
1
2
