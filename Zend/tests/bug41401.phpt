--TEST--
Bug #41401 (wrong precedence for unary minus)
--FILE--
<?php

function fn1947383377()
{
    echo 1 / -2 * 5;
    echo "\n";
    echo 6 / +2 * -3;
}
fn1947383377();
--EXPECT--
-2.5
-9