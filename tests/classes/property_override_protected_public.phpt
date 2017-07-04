--TEST--
Redeclare inherited protected property as public.
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
    public $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn136604897()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn136604897();
--EXPECTF--
A::p
B::p
B::p
