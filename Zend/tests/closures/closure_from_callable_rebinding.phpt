--TEST--
Testing Closure::fromCallable() functionality: Rebinding
--FILE--
<?php

class A
{
    public function method()
    {
        var_dump($this);
    }
}
class B
{
}
function fn1908048603()
{
    $fn = Closure::fromCallable([new A(), 'method']);
    $fn->call(new B());
}
fn1908048603();
--EXPECTF--
Warning: Cannot bind method A::method() to object of class B in %s on line %d
