--TEST--
The variable $php_errormsg shouldn't be optimized as it may be unpredictably modified
--INI--
track_errors=1
--FILE--
<?php

function test()
{
    $php_errormsg = 1;
    echo $undef;
    var_dump($php_errormsg + 1);
}
function fn871267885()
{
    test();
}
fn871267885();
--EXPECTF--
Deprecated: Directive 'track_errors' is deprecated in Unknown on line %d

Notice: Undefined variable: undef in %s on line %d

Warning: A non-numeric value encountered in %s on line %d
int(1)
