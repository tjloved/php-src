--TEST--
Float type should allow an integer as default
--FILE--
<?php

function test(float $arg = 0)
{
    var_dump($arg);
}
function fn1081671923()
{
    test();
}
fn1081671923();
--EXPECT--
float(0)
