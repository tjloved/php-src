--TEST--
Different numbers of arguments in __construct()
--FILE--
<?php

interface foobar
{
    function __construct();
}
abstract class bar implements foobar
{
    public function __construct($x = 1)
    {
    }
}
final class foo extends bar implements foobar
{
    public function __construct($x = 1, $y = 2)
    {
    }
}
function fn1310587407()
{
    new foo();
    print "ok!";
}
fn1310587407();
--EXPECT--
ok!
