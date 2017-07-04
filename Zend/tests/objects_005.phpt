--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test
{
    function &foo()
    {
    }
}
class test2 extends test
{
    function &foo()
    {
    }
}
class test3 extends test
{
    function foo()
    {
    }
}
function fn499482808()
{
    echo "Done\n";
}
fn499482808();
--EXPECTF--	
Warning: Declaration of test3::foo() should be compatible with & test::foo() in %s on line %d
Done
