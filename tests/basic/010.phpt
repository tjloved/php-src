--TEST--
Testing | and & operators
--FILE--
<?php

function fn1098205202()
{
    $a = 8;
    $b = 4;
    $c = 8;
    echo $a | $b & $c;
}
fn1098205202();
--EXPECT--
8
