--TEST--
Testing array with '[]' passed as argument by value
--FILE--
<?php

function test($var)
{
}
function fn211086971()
{
    test($arr[]);
}
fn211086971();
--EXPECTF--
Fatal error: Cannot use [] for reading in %s on line %d
