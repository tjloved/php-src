--TEST--
Writing to a temporary expression is not allowed
--FILE--
<?php

function fn1185204846()
{
    [0, 1][0] = 1;
}
fn1185204846();
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
