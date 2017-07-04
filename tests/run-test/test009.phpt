--TEST--
print_r(Object)
--FILE--
<?php

class Foo
{
}
function fn1015662014()
{
    $foo = new Foo();
    print_r($foo);
}
fn1015662014();
--EXPECTF--
Foo Object
(
)
