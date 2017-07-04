--TEST--
braced complex variable replacement test (heredoc)
--FILE--
<?php

function fn1354811273()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
This is heredoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.

ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
This is heredoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.

ENDOFHEREDOC;
    print "{$x}";
}
fn1354811273();
--EXPECT--
This is heredoc test #s 1, 2, 3, and 4.
This is heredoc test #s 1, 2, 3, and 4.
