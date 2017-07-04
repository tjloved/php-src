--TEST--
Bug #71275 (Bad method called on cloning an object having a trait)
--FILE--
<?php

trait MyTrait
{
    public function _()
    {
        throw new RuntimeException('Should not be called');
    }
}
class MyClass
{
    use MyTrait;
    public function __clone()
    {
        echo "I'm working hard to clone";
    }
}
function fn1199895037()
{
    $instance = new MyClass();
    clone $instance;
}
fn1199895037();
--EXPECT--
I'm working hard to clone
