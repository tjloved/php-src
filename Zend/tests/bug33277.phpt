--TEST--
Bug #33277 (private method accessed by child class)
--FILE--
<?php

class foo
{
    private function bar()
    {
        echo "private!\n";
    }
}
class fooson extends foo
{
    function barson()
    {
        $this->bar();
    }
}
class foo2son extends fooson
{
    function bar()
    {
        echo "public!\n";
    }
}
function fn1830588318()
{
    $b = new foo2son();
    $b->barson();
}
fn1830588318();
--EXPECT--
public!
