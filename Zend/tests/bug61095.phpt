--TEST--
Bug #61095 (Lexing 0x00*+<NUM> incorectly)
--FILE--
<?php

function fn250267963()
{
    echo 0x0 + 2;
    echo "\n";
}
fn250267963();
--EXPECT--
2
