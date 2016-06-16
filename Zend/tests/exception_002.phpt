--TEST--
Testing exception and GOTO
--FILE--
<?php

function fn301550423()
{
    goto foo;
    try {
        print 1;
        foo:
        print 2;
    } catch (Exception $e) {
    }
}
fn301550423();
--EXPECT--
2
