--TEST--
Simple If condition test
--FILE--
<?php

function fn1707288213()
{
    $a = 1;
    if ($a > 0) {
        echo "Yes";
    }
}
fn1707288213();
--EXPECT--
Yes
