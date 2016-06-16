--TEST--
Testing GOTO inside blocks
--FILE--
<?php

function fn414375547()
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
fn414375547();
--EXPECT--
Done!
