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
function fn288179683()
{
    new bar();
    print "OK\n";
}
fn288179683();
--EXPECT--
OK
