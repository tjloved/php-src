--TEST--
Redeclare inherited public property as public static.
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
    public static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn1965078600()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    B::showB();
}
fn1965078600();
--EXPECTF--

Fatal error: Cannot redeclare non static A::$p as static B::$p in %s on line %d
