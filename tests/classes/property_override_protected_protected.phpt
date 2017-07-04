--TEST--
Redeclare inherited protected property as protected.
--FILE--
<?php

class A
{
    protected $p = "A::p";
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
function fn1773476413()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn1773476413();
--EXPECTF--
A::p
B::p
B::p
