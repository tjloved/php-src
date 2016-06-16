--TEST--
Using traits to implement interface
--FILE--
<?php

trait foo
{
    public function abc()
    {
    }
}
interface baz
{
    public function abc();
}
class bar implements baz
{
    use foo;
}
function fn297158564()
{
    new bar();
    print "OK\n";
}
fn297158564();
--EXPECT--
OK
