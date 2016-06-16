--TEST--
jump 10: goto with try blocks
--FILE--
<?php

function fn2065594541()
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
fn2065594541();
--EXPECT--
1234
