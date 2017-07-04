--TEST--
Testing exception and GOTO
--FILE--
<?php

function fn1286117926()
{
    goto foo;
    try {
        print 1;
        foo:
        print 2;
    } catch (Exception $e) {
    }
}
fn1286117926();
--EXPECT--
2
