--TEST--
Redeclare inherited protected property as public static.
--FILE--
<?php

class A
{
    protected $p = "A::p";
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
function fn767789682()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    B::showB();
}
fn767789682();
--EXPECTF--

Fatal error: Cannot redeclare non static A::$p as static B::$p in %s on line %d
