--TEST--
Empty list() assignments are not allowed
--FILE--
<?php

function fn567670087()
{
    list(, , , , , , , , , , ) = [];
}
fn567670087();
--EXPECTF--
Fatal error: Cannot use empty list in %s on line %d
