--TEST--
list with by-reference assignment should fail
--FILE--
<?php

function fn515395326()
{
    $a = [1];
    [&$foo] = $a;
    $foo = 2;
    var_dump($a);
}
fn515395326();
--EXPECTF--
Fatal error: [] and list() assignments cannot be by reference in %s on line %d
