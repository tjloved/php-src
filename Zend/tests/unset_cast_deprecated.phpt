--TEST--
The (unset) cast is deprecated
--FILE--
<?php

function fn689780102()
{
    $x = 1;
    var_dump((unset) $x);
    var_dump($x);
}
fn689780102();
--EXPECTF--
Deprecated: The (unset) cast is deprecated in %s on line %d
NULL
int(1)
