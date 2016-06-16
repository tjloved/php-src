--TEST--
Cannot use built-in functions in write context (reference)
--FILE--
<?php

function fn997937994()
{
    $ref =& strlen("foo");
}
fn997937994();
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
