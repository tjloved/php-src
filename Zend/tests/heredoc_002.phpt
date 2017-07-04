--TEST--
basic binary heredoc syntax
--FILE--
<?php

function fn1417667069()
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
fn1417667069();
--EXPECT--
This is a heredoc test.
This is another heredoc test.
