--TEST--
It's not possible to increment the return value of a function
--FILE--
<?php

function test()
{
    return 42;
}
function fn446630238()
{
    ++test();
}
fn446630238();
--EXPECTF--
Fatal error: Can't use function return value in write context in %s on line %d
