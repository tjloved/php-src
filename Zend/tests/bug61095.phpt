--TEST--
Bug #61095 (Lexing 0x00*+<NUM> incorectly)
--FILE--
<?php

function fn1021886017()
{
    echo 0x0 + 2;
    echo "\n";
}
fn1021886017();
--EXPECT--
2
