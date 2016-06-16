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
function fn2108257148()
{
    $a = A::foo();
    $a->bindTo(new A());
    echo "Done.\n";
}
fn2108257148();
--EXPECTF--
Done.
