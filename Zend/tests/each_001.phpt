--TEST--
Testing each() with an undefined variable
--FILE--
<?php

function fn2142614228()
{
    each($foo);
}
fn2142614228();
--EXPECTF--
Deprecated: The each() function is deprecated. This message will be suppressed on further calls in %s on line %d

Warning: Variable passed to each() is not an array or object in %s on line %d
