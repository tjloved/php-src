--TEST--
Call internal function with incorrect number of arguments
--FILE--
<?php

function fn48518724()
{
    substr("foo");
    array_diff([]);
}
fn48518724();
--EXPECTF--
Warning: substr() expects at least 2 parameters, 1 given in %s

Warning: array_diff(): at least 2 parameters are required, 1 given in %s