--TEST--
Writing to a temporary expression is not allowed
--FILE--
<?php

function fn2128661011()
{
    [0, 1][0] = 1;
}
fn2128661011();
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
