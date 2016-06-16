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
function fn194539716()
{
    $f = (new A())->f();
    var_dump($f->bindTo(null, 'B')());
}
fn194539716();
--EXPECT--
string(4) "B::C"
