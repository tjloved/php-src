--TEST--
empty doc test (heredoc)
--FILE--
<?php

function fn1015345595()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
ENDOFHEREDOC;
    print "{$x}";
}
fn1015345595();
--EXPECT--
