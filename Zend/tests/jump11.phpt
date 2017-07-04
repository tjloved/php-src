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
function fn39571207()
{
    new foobar();
}
fn39571207();
--EXPECT--
ok!
ok!
ok!
