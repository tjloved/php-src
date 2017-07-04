--TEST--
simple variable replacement test (heredoc)
--FILE--
<?php

function fn339436602()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
This is heredoc test #{$a}.

ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
This is heredoc test #{$b}.

ENDOFHEREDOC;
    print "{$x}";
}
fn339436602();
--EXPECT--
This is heredoc test #1.
This is heredoc test #2.
