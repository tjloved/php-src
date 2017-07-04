--TEST--
Testing GOTO inside blocks
--FILE--
<?php

function fn1971881232()
{
    goto A;
    B:
    goto C;
    return;
    A:
    goto B;
    C:
    print "Done!\n";
}
fn1971881232();
--EXPECT--
Done!
