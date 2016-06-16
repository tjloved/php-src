--TEST--
jump 08: goto inside switch in constructor
--FILE--
<?php

class foobar
{
    public function __construct()
    {
        switch (1) {
            default:
                goto b;
                a:
                print "ok!\n";
                break;
                b:
                print "ok!\n";
                goto a;
        }
        print "ok!\n";
    }
}
function fn1489976925()
{
    new foobar();
}
fn1489976925();
--EXPECT--
ok!
ok!
ok!
