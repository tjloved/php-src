--TEST--
jump 17: goto into try/catch with finally
--FILE--
<?php

function fn1334693143()
{
    goto b;
    try {
        echo "1";
        a:
        echo "2";
        throw new Exception();
    } catch (Exception $e) {
        echo "3";
        b:
        echo "4";
    } finally {
        echo "5";
        c:
        echo "6";
    }
    echo "7\n";
}
fn1334693143();
--EXPECT--
4567
