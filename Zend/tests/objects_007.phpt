--TEST--
method overloading with different method signature
--INI--
error_reporting=8191
--FILE--
<?php

class test
{
    function foo($arg, &$arg2 = NULL)
    {
    }
}
class test2 extends test
{
    function foo($arg, &$arg2 = NULL)
    {
    }
}
class test3 extends test
{
    function foo($arg, &$arg2)
    {
    }
}
function fn1747161682()
{
    echo "Done\n";
}
fn1747161682();
--EXPECTF--	
Warning: Declaration of test3::foo($arg, &$arg2) should be compatible with test::foo($arg, &$arg2 = NULL) in %s on line %d
Done
