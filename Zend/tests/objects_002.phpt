--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test
{
    function foo()
    {
    }
}
class test2 extends test
{
    function foo()
    {
    }
}
class test3 extends test
{
    function foo($arg)
    {
    }
}
function fn1008385500()
{
    echo "Done\n";
}
fn1008385500();
--EXPECTF--	
Warning: Declaration of test3::foo($arg) should be compatible with test::foo() in %s on line %d
Done
