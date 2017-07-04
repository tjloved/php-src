--TEST--
Catching an exception thrown from an included file
--FILE--
<?php

function fn2126998798()
{
    try {
        include "inc_throw.inc";
    } catch (Exception $e) {
        echo "caught exception\n";
    }
}
fn2126998798();
--EXPECT--
caught exception
