--TEST--
Cannot use empty elements in keyed array destructuring
--FILE--
<?php

function fn2126210345()
{
    $array = ['a' => 1, 'b' => 2];
    ['a' => $a, , 'b' => $b] = $array;
}
fn2126210345();
--EXPECTF--
Fatal error: Cannot use empty array entries in keyed array assignment in %s on line %d
