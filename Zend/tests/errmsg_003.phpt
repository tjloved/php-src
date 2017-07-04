--TEST--
errmsg: cannot reassign $this (by ref)
--FILE--
<?php

class test
{
    function foo()
    {
        $a = new test();
        $this =& $a;
    }
}
function fn994492385()
{
    $t = new test();
    $t->foo();
    echo "Done\n";
}
fn994492385();
--EXPECTF--	
Fatal error: Cannot re-assign $this in %s on line %d
