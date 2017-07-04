--TEST--
Testing get_parent_class()
--FILE--
<?php

interface ITest
{
    function foo();
}
abstract class bar implements ITest
{
    public function foo()
    {
        var_dump(get_parent_class());
    }
}
class foo extends bar
{
    public function __construct()
    {
        var_dump(get_parent_class());
    }
}
function fn1771182396()
{
    $a = new foo();
    $a->foo();
}
fn1771182396();
--EXPECT--
string(3) "bar"
bool(false)
