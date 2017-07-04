--TEST--
Can't use name of lexical variable for parameter
--FILE--
<?php

function fn2027724199()
{
    $a = 1;
    $fn = function ($a) use($a) {
        var_dump($a);
    };
    $fn(2);
}
fn2027724199();
--EXPECTF--
Fatal error: Cannot use lexical variable $a as a parameter name in %s on line %d
