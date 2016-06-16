--TEST--
Testing each() with an undefined variable
--FILE--
<?php

function fn116843982()
{
    each($foo);
}
fn116843982();
--EXPECTF--
Warning: Variable passed to each() is not an array or object in %s on line %d
