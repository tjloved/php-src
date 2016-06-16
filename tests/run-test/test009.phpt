--TEST--
print_r(Object)
--FILE--
<?php

class Foo
{
}
function fn354992772()
{
    $foo = new Foo();
    print_r($foo);
}
fn354992772();
--EXPECTF--
Foo Object
(
)
