--TEST--
Redeclare inherited private property as protected.
--FILE--
<?php

class A
{
    private $p = "A::p";
    function showA()
    {
        echo $this->p . "\n";
    }
}
class B extends A
{
    protected $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn1764017729()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn1764017729();
--EXPECTF--
A::p
A::p
B::p
