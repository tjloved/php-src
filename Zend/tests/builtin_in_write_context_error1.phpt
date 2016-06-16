--TEST--
Cannot use built-in functions in write context (assignment)
--FILE--
<?php

function fn531909667()
{
    strlen("foo")[0] = 1;
}
fn531909667();
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
