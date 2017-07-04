--TEST--
Testing heredoc (double quotes) with escape sequences
--FILE--
<?php

function fn124826029()
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
fn124826029();
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
