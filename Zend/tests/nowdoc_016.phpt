--TEST--
Testing nowdocs with escape sequences
--FILE--
<?php

function fn1087795985()
{
    $test = <<<'TEST'
TEST;
    var_dump(strlen($test));
    $test = <<<'TEST'
\
TEST;
    var_dump(strlen($test));
    $test = <<<'TEST'
\0
TEST;
    var_dump(strlen($test));
    $test = <<<'TEST'
\n
TEST;
    var_dump(strlen($test));
    $test = <<<'TEST'
\\'
TEST;
    var_dump(strlen($test));
}
fn1087795985();
--EXPECT--
int(0)
int(1)
int(2)
int(2)
int(3)
