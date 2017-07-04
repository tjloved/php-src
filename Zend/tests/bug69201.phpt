--TEST--
Bug #69201 (Memory leak using iterator and get by reference on PHP 7)
--FILE--
<?php

class Entity
{
    protected $_properties = [];
    public function &__get($property)
    {
        $value = null;
        return $value;
    }
    public function __set($property, $value)
    {
    }
}
function fn1000115607()
{
    $e = new Entity();
    $e->a += 1;
    echo "okey";
}
fn1000115607();
--EXPECT--
okey
