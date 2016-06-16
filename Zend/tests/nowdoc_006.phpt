--TEST--
braced complex variable replacement test (nowdoc)
--FILE--
<?php

function fn10102147()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
This is nowdoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
This is nowdoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.

ENDOFNOWDOC;
    print "{$x}";
}
fn10102147();
--EXPECT--
This is nowdoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.
This is nowdoc test #s {$a}, {$b}, {$c['c']}, and {$d->d}.
