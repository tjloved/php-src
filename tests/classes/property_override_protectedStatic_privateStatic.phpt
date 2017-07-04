--TEST--
Redeclare inherited protected static property as private static.
--FILE--
<?php

class A
{
    protected static $p = "A::p (static)";
    static function showA()
    {
        echo self::$p . "\n";
    }
}
class B extends A
{
    private static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn1050129341()
{
    A::showA();
    B::showA();
    B::showB();
}
fn1050129341();
--EXPECTF--

Fatal error: Access level to B::$p must be protected (as in class A) or weaker in %s on line %d

