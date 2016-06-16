--TEST--
braces variable replacement test (nowdoc)
--FILE--
<?php

function fn1219314591()
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
fn1219314591();
--EXPECT--
This is nowdoc test #{$a}.
This is nowdoc test #{$b}.
