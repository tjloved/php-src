--TEST--
030: Name ambiguity (import name & class name)
--FILE--
<?php

class Foo
{
}
use A\B as Foo;
function fn1846512572()
{
    new Foo();
}
fn1846512572();
--EXPECTF--
Fatal error: Cannot use A\B as Foo because the name is already in use in %sns_030.php on line %d
