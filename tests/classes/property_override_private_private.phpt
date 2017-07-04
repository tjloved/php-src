--TEST--
Redeclare inherited private property as private.
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
    private $p = "B::p";
    function showB()
    {
        echo $this->p . "\n";
    }
}
function fn1172751936()
{
    $a = new A();
    $a->showA();
    $b = new B();
    $b->showA();
    $b->showB();
}
fn1172751936();
--EXPECTF--
A::p
A::p
B::p
