--TEST--
Duplicate labels are not allowed
--FILE--
<?php

function fn1517624663()
{
    foo:
    foo:
    goto foo;
}
fn1517624663();
--EXPECTF--
Fatal error: Label 'foo' already defined in %s on line %d
