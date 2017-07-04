--TEST--
Cannot use built-in functions in write context (assignment)
--FILE--
<?php

function fn1700057641()
{
    strlen("foo")[0] = 1;
}
fn1700057641();
--EXPECTF--
Fatal error: Cannot use result of built-in function in write context in %s on line %d
