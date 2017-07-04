--TEST--
Class method registration
--FILE--
<?php

class A
{
    function foo()
    {
    }
}
class B extends A
{
    function foo()
    {
    }
}
class C extends B
{
    function foo()
    {
    }
}
class D extends A
{
}
class F extends D
{
    function foo()
    {
    }
}
function fn1052530635()
{
    // Following class definition should fail, but cannot test
    /*
    class X {
    	function foo() {}
    	function foo() {}
    }
    */
    echo "OK\n";
}
fn1052530635();
--EXPECT--
OK

