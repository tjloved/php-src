--TEST--
Closure::__invoke() is case insensitive
--FILE--
<?php

function fn1345658981()
{
    $inc = function (&$n) {
        $n++;
    };
    $n = 1;
    $inc->__INVOKE($n);
    var_dump($n);
}
fn1345658981();
--EXPECT--
int(2)
