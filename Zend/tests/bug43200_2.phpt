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
function fn661433209()
{
    $o = new C();
    $o->foo();
}
fn661433209();
--EXPECTF--
works
