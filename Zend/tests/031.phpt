--TEST--
Testing array with '[]' passed as argument by value
--FILE--
<?php

function test($var)
{
}
function fn475869301()
{
    test($arr[]);
}
fn475869301();
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
