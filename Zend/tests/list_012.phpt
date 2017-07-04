--TEST--
Disallow empty elements in normal arrays
--FILE--
<?php

function fn482633945()
{
    var_dump([, 1, 2]);
}
fn482633945();
--EXPECTF--
Fatal error: Cannot use empty array elements in arrays in %s on line %d
