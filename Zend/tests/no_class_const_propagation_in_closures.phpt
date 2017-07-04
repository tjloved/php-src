--TEST--
self:: class constants should not be propagated into closures, due to scope rebinding
--FILE--
<?php

class A
{
    const C = 'A::C';
    public function f()
    {
        return function () {
            return self::C;
        };
    }
}
class B
{
    const C = 'B::C';
}
function fn2003647601()
{
    $f = (new A())->f();
    var_dump($f->bindTo(null, 'B')());
}
fn2003647601();
--EXPECT--
string(4) "B::C"
