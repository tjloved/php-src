--TEST--
Can't use name of lexical variable for parameter
--FILE--
<?php

function fn242285403()
{
    $a = 1;
    $fn = function ($a) use($a) {
        var_dump($a);
    };
    $fn(2);
}
fn242285403();
--EXPECTF--
Fatal error: Cannot use lexical variable $a as a parameter name in %s on line %d
