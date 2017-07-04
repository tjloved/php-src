--TEST--
Testing heredoc with tabs before identifier
--FILE--
<?php

function fn1817533850()
{
    $heredoc = <<<A

foo

\tA;
A;
    var_dump(strlen($heredoc) == 9);
}
fn1817533850();
--EXPECT--
bool(true)
