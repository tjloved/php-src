--TEST--
ZE2 An abstract class must be declared abstract
--FILE--
<?php

class fail
{
    abstract function show();
}
function fn1388993539()
{
    echo "Done\n";
    // shouldn't be displayed
}
fn1388993539();
--EXPECTF--
Fatal error: Class fail contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (fail::show) in %s on line %d
