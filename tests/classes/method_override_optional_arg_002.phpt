--TEST--
Omitting optional arg in method inherited from abstract class 
--FILE--
<?php

abstract class A
{
    function foo($arg = 1)
    {
    }
}
class B extends A
{
    function foo()
    {
        echo "foo\n";
    }
}
function fn2078329687()
{
    $b = new B();
    $b->foo();
}
fn2078329687();
--EXPECTF--
Warning: Declaration of B::foo() should be compatible with A::foo($arg = 1) in %s on line %d
foo
