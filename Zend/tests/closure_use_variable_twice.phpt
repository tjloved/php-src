--TEST--
Closure cannot use one variable twice
--FILE--
<?php

function fn1791010046()
{
    $a = 1;
    $fn = function () use($a, &$a) {
        $a = 2;
    };
    $fn();
    var_dump($a);
}
fn1791010046();
--EXPECTF--
Fatal error: Cannot use variable $a twice in %s on line %d
