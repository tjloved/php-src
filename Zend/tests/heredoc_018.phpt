--TEST--
Testing heredoc with tabs before identifier
--FILE--
<?php

function fn1794649443()
{
    $heredoc = <<<A

foo

\tA;
A;
    var_dump(strlen($heredoc) == 9);
}
fn1794649443();
--EXPECT--
bool(true)
