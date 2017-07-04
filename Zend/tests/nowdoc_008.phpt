--TEST--
empty doc test (nowdoc)
--FILE--
<?php

function fn1533187142()
{
    require_once 'nowdoc.inc';
    print <<<'ENDOFNOWDOC'
ENDOFNOWDOC;
    $x = <<<'ENDOFNOWDOC'
ENDOFNOWDOC;
    print "{$x}";
}
fn1533187142();
--EXPECT--
