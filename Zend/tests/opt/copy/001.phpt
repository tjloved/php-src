--TEST--
Copy 001
--FILE--
<?php

function test()
{
    $b = $a;
    var_dump($b);
    var_dump($b);
}
function fn1421173144()
{
    test();
}
fn1421173144();
--EXPECTF--
Notice: Undefined variable: a in %s on line %d
NULL
NULL
