--TEST--
Closure 050: static::class in non-static closure in non-static method.

--FILE--
<?php

class A
{
    function foo()
    {
        $f = function () {
            return static::class;
        };
        return $f();
    }
}
class B extends A
{
}
function fn976714332()
{
    $b = new B();
    var_dump($b->foo());
}
fn976714332();
--EXPECT--
string(1) "B"
