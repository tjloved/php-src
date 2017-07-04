--TEST--
basic heredoc syntax
--FILE--
<?php

function fn1848727674()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
This is a heredoc test.

ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
This is another heredoc test.

ENDOFHEREDOC;
    print "{$x}";
}
fn1848727674();
--EXPECT--
This is a heredoc test.
This is another heredoc test.
