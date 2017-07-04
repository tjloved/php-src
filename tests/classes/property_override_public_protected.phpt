--TEST--
Redeclare inherited public property as protected.
--FILE--
<?php

class A
{
    public $p = "A::p";
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
function fn1452536961()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn1452536961();
--EXPECTF--

Fatal error: Access level to B::$p must be public (as in class A) in %s on line %d

