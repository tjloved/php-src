--TEST--
Float type should allow an integer as default
--FILE--
<?php

function test(float $arg = 0)
{
    var_dump($arg);
}
function fn1565354954()
{
    test();
}
fn1565354954();
--EXPECT--
float(0)
