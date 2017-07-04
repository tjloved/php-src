--TEST--
Duplicate labels are not allowed
--FILE--
<?php

function fn1695132487()
{
    foo:
    foo:
    goto foo;
}
fn1695132487();
--EXPECTF--
Fatal error: Label 'foo' already defined in %s on line %d
