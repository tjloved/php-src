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
function fn2060338322()
{
    test();
}
fn2060338322();
--EXPECTF--
Notice: Undefined variable: a in %s on line %d
NULL
NULL
