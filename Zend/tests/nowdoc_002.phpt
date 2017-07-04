--TEST--
basic binary nowdoc syntax
--FILE--
<?php

function fn1121083442()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
This is a nowdoc test.

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
This is another nowdoc test.

ENDOFNOWDOC;
    print "{$x}";
}
fn1121083442();
--EXPECT--
This is a nowdoc test.
This is another nowdoc test.
