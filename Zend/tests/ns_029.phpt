--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php

use A\B as Foo;
class Foo
{
}
function fn1509584997()
{
    new Foo();
}
fn1509584997();
--EXPECTF--
Fatal error: Cannot declare class Foo because the name is already in use in %sns_029.php on line %d
