--TEST--
Cannot use built-in functions in write context (reference)
--FILE--
<?php

function fn2025311803()
{
    $ref =& strlen("foo");
}
fn2025311803();
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
