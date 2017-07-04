--TEST--
Test to check regressions on use statements and lexer state
--FILE--
<?php

use A\B\C\D;
class Foo
{
    private static $foo;
}
function fn275536186()
{
    echo PHP_EOL, "Done", PHP_EOL;
}
fn275536186();
--EXPECTF--

Done
