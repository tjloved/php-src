--TEST--
Bug #72177 Scope issue in __destruct after ReflectionProperty::setValue()
--FILE--
<?php

class Child
{
    protected $bar;
    public function __destruct()
    {
        $this->bar = null;
    }
}
class Parnt
{
    protected $child;
    public function doSomething()
    {
        $this->child = new Child();
        $prop = new \ReflectionProperty($this, 'child');
        $prop->setAccessible(true);
        $prop->setValue($this, null);
    }
}
function fn637633627()
{
    $p = new Parnt();
    $p->doSomething();
    echo "OK\n";
}
fn637633627();
--EXPECT--
OK
