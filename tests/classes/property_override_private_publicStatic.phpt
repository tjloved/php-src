--TEST--
Redeclare inherited private property as public static.
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
    public static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn311170817()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    B::showB();
}
fn311170817();
--EXPECTF--
A::p
A::p
B::p (static)
