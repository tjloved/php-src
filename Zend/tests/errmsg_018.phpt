--TEST--
errmsg: static abstract function 
--FILE--
<?php

class test
{
    static abstract function foo();
}
function fn1769982116()
{
    echo "Done\n";
}
fn1769982116();
--EXPECTF--	

Fatal error: Class test contains 1 abstract method and must therefore be declared abstract or implement the remaining methods (test::foo) in %s on line %d
