--TEST--
Bug #51791 (constant() failed to check undefined constant and php interpreter stoped)
--FILE--
<?php

class A
{
    const B = 1;
}
function fn160455172()
{
    var_dump(constant('A::B1'));
}
fn160455172();
--EXPECTF--
Warning: constant(): Couldn't find constant A::B1 in %s on line %d
NULL
