--TEST--
testing @ and error_reporting - 6
--FILE--
<?php

function foo1($arg)
{
}
function foo2($arg)
{
}
function foo3()
{
    echo $undef3;
    throw new Exception("test");
}
function fn1419826942()
{
    error_reporting(E_ALL);
    try {
        @foo1(@foo2(@foo3()));
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn1419826942();
--EXPECTF--	
int(32767)
Done
