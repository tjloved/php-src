--TEST--
simple variable replacement test (nowdoc)
--FILE--
<?php

function fn120572818()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
This is nowdoc test #$a.

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
This is nowdoc test #$b.

ENDOFNOWDOC;
    print "{$x}";
}
fn120572818();
--EXPECT--
This is nowdoc test #$a.
This is nowdoc test #$b.
