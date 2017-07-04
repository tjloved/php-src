--TEST--
Testing heredoc with escape sequences
--FILE--
<?php

function fn1150487908()
{
    $test = <<<TEST
TEST;
    var_dump(strlen($test) == 0);
    $test = <<<TEST
\\
TEST;
    var_dump(strlen($test) == 1);
    $test = <<<TEST
\0
TEST;
    var_dump(strlen($test) == 1);
    $test = <<<TEST


TEST;
    var_dump(strlen($test) == 1);
    $test = <<<TEST
\\'
TEST;
    var_dump(strlen($test) == 2);
}
fn1150487908();
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
