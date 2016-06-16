--TEST--
Catching an exception thrown from an included file
--FILE--
<?php

function fn1267804494()
{
    try {
        include "inc_throw.inc";
    } catch (Exception $e) {
        echo "caught exception\n";
    }
}
fn1267804494();
--EXPECT--
caught exception
