--TEST--
Bug #61225 (Lexing 0b0*+<NUM> incorectly)
--FILE--
<?php

function fn2032791399()
{
    echo 0b0 + 1;
    echo "\n";
}
fn2032791399();
--EXPECT--
1
