--TEST--
Bug 69124: Method name must be as string (invalid error message when using reference to a string)
--FILE--
<?php

class Foo
{
    public function bar()
    {
        print "Success\n";
    }
}
function test(&$instance, &$method)
{
    $instance->{$method}();
}
function fn888746275()
{
    $instance = new Foo();
    $method = "bar";
    test($instance, $method);
}
fn888746275();
--EXPECT--
Success
