--TEST--
empty doc test (nowdoc)
--FILE--
<?php

function fn1731769319()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
ENDOFNOWDOC;
    print "{$x}";
}
fn1731769319();
--EXPECT--
