--TEST--
Redeclare inherited private property as public.
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
    public $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn2008427830()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn2008427830();
--EXPECTF--
A::p
A::p
B::p
