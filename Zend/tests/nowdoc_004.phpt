--TEST--
braces variable replacement test (nowdoc)
--FILE--
<?php

function fn795484829()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
This is nowdoc test #{$a}.

ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
This is nowdoc test #{$b}.

ENDOFNOWDOC;
    print "{$x}";
}
fn795484829();
--EXPECT--
This is nowdoc test #{$a}.
This is nowdoc test #{$b}.
