--TEST--
Redeclare inherited private static property as private.
--FILE--
<?php

class A
{
    private static $p = "A::p (static)";
    static function showA()
    {
        echo self::$p . "\n";
    }
}
class B extends A
{
    private $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn2100512719()
{
    A::showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn2100512719();
--EXPECTF--
A::p (static)
A::p (static)
B::p
