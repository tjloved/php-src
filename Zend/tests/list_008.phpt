--TEST--
Assignment to invalid list() value
--FILE--
<?php

function fn1136701330()
{
    [42] = [1];
}
fn1136701330();
--EXPECTF--
Fatal error: Assignments can only happen to writable values in %s on line %d
