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
function fn591807229()
{
    A::showA();
    B::showA();
    B::showB();
}
fn591807229();
--EXPECTF--
A::p (static)
A::p (static)
B::p (static)
