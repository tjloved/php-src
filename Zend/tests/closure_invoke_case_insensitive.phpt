--TEST--
Closure::__invoke() is case insensitive
--FILE--
<?php

function fn182534463()
{
    $inc = function (&$n) {
        $n++;
    };
    $n = 1;
    $inc->__INVOKE($n);
    var_dump($n);
}
fn182534463();
--EXPECT--
int(2)
