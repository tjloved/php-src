--TEST--
basic binary heredoc syntax
--FILE--
<?php

function fn382840077()
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
fn382840077();
--EXPECT--
This is a heredoc test.
This is another heredoc test.
