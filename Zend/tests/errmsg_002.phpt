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
function fn2720426()
{
    echo "Done\n";
}
fn2720426();
--EXPECTF--	
Fatal error: Abstract function test::foo() cannot be declared private in %s on line %d
