--TEST--
Bug #43200.2 (Interface implementation / inheritance not possible in abstract classes)
--FILE--
<?php

interface A
{
    function foo();
}
abstract class B implements A
{
    public abstract function foo();
}
class C extends B
{
    public function foo()
    {
        echo 'works';
    }
}
function fn1326293819()
{
    $o = new C();
    $o->foo();
}
fn1326293819();
--EXPECTF--
works
