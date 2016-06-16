--TEST--
basic nowdoc syntax
--FILE--
<?php

function fn856012528()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
This is a nowdoc test.

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
This is another nowdoc test.
With another line in it.
ENDOFNOWDOC;
    print "{$x}";
}
fn856012528();
--EXPECT--
This is a nowdoc test.
This is another nowdoc test.
With another line in it.
