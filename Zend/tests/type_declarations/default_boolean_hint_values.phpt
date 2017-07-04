--TEST--
Default values for boolean hints should work
--FILE--
<?php

function foo(bool $x = true, bool $y = false)
{
    var_dump($x, $y);
}
function fn712056562()
{
    foo();
}
fn712056562();
--EXPECTF--
bool(true)
bool(false)
