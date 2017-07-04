--TEST--
Redeclare inherited private static property as public.
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
    public $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn983068049()
{
    A::showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn983068049();
--EXPECTF--
A::p (static)
A::p (static)
B::p
