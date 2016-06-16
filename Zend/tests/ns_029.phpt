--TEST--
029: Name ambiguity (class name & import name)
--FILE--
<?php

use A\B as Foo;
class Foo
{
}
function fn1244050784()
{
    new Foo();
}
fn1244050784();
--EXPECTF--
Fatal error: Cannot declare class Foo because the name is already in use in %sns_029.php on line %d
