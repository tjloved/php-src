--TEST--
Bug #61225 (Lexing 0b0*+<NUM> incorectly)
--FILE--
<?php

function fn1510874446()
{
    echo 0b0 + 1;
    echo "\n";
}
fn1510874446();
--EXPECT--
1
