--TEST--
Default values for boolean hints should work
--FILE--
<?php

function foo(bool $x = true, bool $y = false)
{
    var_dump($x, $y);
}
function fn1805127805()
{
    foo();
}
fn1805127805();
--EXPECTF--
bool(true)
bool(false)
