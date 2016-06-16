--TEST--
Positional arguments cannot be used after argument unpacking
--FILE--
<?php

function fn699048673()
{
    var_dump(...[1, 2, 3], 4);
}
fn699048673();
--EXPECTF--
Fatal error: Cannot use positional argument after argument unpacking in %s on line %d
