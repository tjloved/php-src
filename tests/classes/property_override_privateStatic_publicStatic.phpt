--TEST--
Redeclare inherited private static property as public static.
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
    public static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn1470206236()
{
    A::showA();
    B::showA();
    B::showB();
}
fn1470206236();
--EXPECTF--
A::p (static)
A::p (static)
B::p (static)
