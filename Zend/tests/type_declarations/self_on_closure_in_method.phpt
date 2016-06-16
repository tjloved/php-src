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
function fn2042782964()
{
    var_dump((new A())->test()());
}
fn2042782964();
--EXPECT--
object(A)#1 (0) {
}
