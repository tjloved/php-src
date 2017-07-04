--TEST--
Closure cannot use one variable twice
--FILE--
<?php

function fn528662840()
{
    $a = 1;
    $fn = function () use($a, &$a) {
        $a = 2;
    };
    $fn();
    var_dump($a);
}
fn528662840();
--EXPECTF--
Fatal error: Cannot use variable $a twice in %s on line %d
