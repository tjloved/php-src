--TEST--
Redeclare inherited private property as protected static.
--FILE--
<?php

class A
{
    private $p = "A::p";
    function showA()
    {
        echo $this->p . "\n";
    }
}
class B extends A
{
    protected static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn855594415()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    B::showB();
}
fn855594415();
--EXPECTF--
A::p
A::p
B::p (static)
