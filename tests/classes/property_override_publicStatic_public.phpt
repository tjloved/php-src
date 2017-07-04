--TEST--
Redeclare inherited public static property as public.
--FILE--
<?php

class A
{
    public static $p = "A::p (static)";
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
function fn341671487()
{
    A::showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn341671487();
--EXPECTF--

Fatal error: Cannot redeclare static A::$p as non static B::$p in %s on line %d

