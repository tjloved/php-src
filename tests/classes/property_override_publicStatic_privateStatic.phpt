--TEST--
Redeclare inherited public static property as private static.
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
    private static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn1231356533()
{
    A::showA();
    B::showA();
    B::showB();
}
fn1231356533();
--EXPECTF--

Fatal error: Access level to B::$p must be public (as in class A) in %s on line %d

