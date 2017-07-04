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
function fn1199958387()
{
    A::showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn1199958387();
--EXPECTF--
A::p (static)
A::p (static)
B::p
