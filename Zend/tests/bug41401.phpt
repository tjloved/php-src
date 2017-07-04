--TEST--
Bug #41401 (wrong precedence for unary minus)
--FILE--
<?php

function fn574155179()
{
    echo 1 / -2 * 5;
    echo "\n";
    echo 6 / +2 * -3;
}
fn574155179();
--EXPECT--
-2.5
-9