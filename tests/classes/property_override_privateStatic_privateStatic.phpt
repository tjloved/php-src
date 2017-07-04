--TEST--
Redeclare inherited private static property as private static.
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
    private static $p = "B::p (static)";
    static function showB()
    {
        echo self::$p . "\n";
    }
}
function fn890910974()
{
    A::showA();
    B::showA();
    B::showB();
}
fn890910974();
--EXPECTF--
A::p (static)
A::p (static)
B::p (static)
