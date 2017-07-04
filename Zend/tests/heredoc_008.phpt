--TEST--
empty doc test (heredoc)
--FILE--
<?php

function fn9986480()
{
    require_once 'nowdoc.inc';
    print <<<ENDOFHEREDOC
ENDOFHEREDOC;
    $x = <<<ENDOFHEREDOC
ENDOFHEREDOC;
    print "{$x}";
}
fn9986480();
--EXPECT--
