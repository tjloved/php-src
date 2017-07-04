--TEST--
Bug #29883 (isset gives invalid values on strings)
--FILE--
<?php

function fn1986165041()
{
    $x = "bug";
    var_dump(isset($x[-10]));
    var_dump(isset($x["1"]));
    echo $x["1"] . "\n";
}
fn1986165041();
--EXPECT--
bool(false)
bool(true)
u
