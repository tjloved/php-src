--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test
{
    function foo(Test $arg)
    {
    }
}
class test2 extends test
{
    function foo(Test $arg)
    {
    }
}
class test3 extends test
{
    function foo($arg)
    {
    }
}
function fn1228182507()
{
    echo "Done\n";
}
fn1228182507();
--EXPECTF--	
Warning: Declaration of test3::foo($arg) should be compatible with test::foo(Test $arg) in %s on line %d
Done
