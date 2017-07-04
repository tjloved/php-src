--TEST--
Redeclare inherited public property as public.
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
    public $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn874272122()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn874272122();
--EXPECTF--
A::p
B::p
B::p
