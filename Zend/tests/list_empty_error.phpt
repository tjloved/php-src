--TEST--
Empty list() assignments are not allowed
--FILE--
<?php

function fn66290439()
{
    list(, , , , , , , , , , ) = [];
}
fn66290439();
--EXPECTF--
Fatal error: Cannot use empty list in %s on line %d
