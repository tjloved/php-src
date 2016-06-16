--TEST--
Testing | and & operators
--FILE--
<?php

function fn848643936()
{
    $a = 8;
    $b = 4;
    $c = 8;
    echo $a | $b & $c;
}
fn848643936();
--EXPECT--
8
