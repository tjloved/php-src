--TEST--
Float type should allow an integer as default even with strict types
--FILE--
<?php

declare (strict_types=1);
function test(float $arg = 0)
{
    var_dump($arg);
}
function fn751421745()
{
    test();
}
fn751421745();
--EXPECT--
float(0)
