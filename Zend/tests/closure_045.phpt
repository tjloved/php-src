--TEST--
Closure 045: Closures created in static methods are not implicitly static
--FILE--
<?php

class A
{
    static function foo()
    {
        return function () {
        };
    }
}
function fn1175340988()
{
    $a = A::foo();
    $a->bindTo(new A());
    echo "Done.\n";
}
fn1175340988();
--EXPECTF--
Done.
