--TEST--
jump 10: goto with try blocks
--FILE--
<?php

function fn2108501471()
{
    goto a;
    e:
    return;
    try {
        a:
        print 1;
        goto b;
        try {
            b:
            print 2;
            goto c;
        } catch (Exception $e) {
            c:
            print 3;
            goto d;
        }
    } catch (Exception $e) {
        d:
        print 4;
        goto e;
    }
}
fn2108501471();
--EXPECT--
1234
