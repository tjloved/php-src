--TEST--
errmsg: function cannot be declared private
--FILE--
<?php

abstract class test
{
    private abstract function foo()
    {
    }
}
function fn1397752548()
{
    echo "Done\n";
}
fn1397752548();
--EXPECTF--	
Fatal error: Abstract function test::foo() cannot be declared private in %s on line %d
