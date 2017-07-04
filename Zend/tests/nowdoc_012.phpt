--TEST--
Test false labels
--FILE--
<?php

function fn685055291()
{
    require_once 'nowdoc.inc';
    $x = <<<'ENDOFNOWDOC'
This is a nowdoc test.
NOTREALLYEND;
Another line
NOTENDEITHER;
ENDOFNOWDOCWILLBESOON
Now let's finish it
ENDOFNOWDOC;
    print "{$x}\n";
}
fn685055291();
--EXPECT--
This is a nowdoc test.
NOTREALLYEND;
Another line
NOTENDEITHER;
ENDOFNOWDOCWILLBESOON
Now let's finish it
