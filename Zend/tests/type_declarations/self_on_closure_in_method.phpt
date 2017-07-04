--TEST--
self return type on closure in a method
--FILE--
<?php

class A
{
    public function test()
    {
        return function () : self {
            return $this;
        };
    }
}
function fn260805209()
{
    var_dump((new A())->test()());
}
fn260805209();
--EXPECT--
object(A)#1 (0) {
}
