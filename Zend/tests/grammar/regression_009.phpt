--TEST--
Test to check regressions on use statements and lexer state
--FILE--
<?php

use A\B\C\D;
class Foo
{
    private static $foo;
}
function fn1239295229()
{
    echo PHP_EOL, "Done", PHP_EOL;
}
fn1239295229();
--EXPECTF--

Done
