--TEST--
Bug #60217 (Requiring the same method from different traits.)
--FILE--
<?php

trait T1
{
    public abstract function foo();
}
trait T2
{
    public abstract function foo();
}
class C
{
    use T1, T2;
    public function foo()
    {
        echo "C::foo() works.\n";
    }
}
function fn534320181()
{
    $o = new C();
    $o->foo();
}
fn534320181();
--EXPECTF--
C::foo() works.
