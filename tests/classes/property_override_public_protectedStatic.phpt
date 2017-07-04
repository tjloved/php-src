--TEST--
Redeclare inherited public property as protected static.
--FILE--
<?php

class A
{
    public $p = "A::p";
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
function fn471964676()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    B::showB();
}
fn471964676();
--EXPECTF--

Fatal error: Cannot redeclare non static A::$p as static B::$p in %s on line %d
