--TEST--
testing @ and error_reporting - 1
--FILE--
<?php

function foo($arg)
{
}
function bar()
{
    throw new Exception("test");
}
function fn1124910777()
{
    error_reporting(E_ALL);
    try {
        @foo(@bar());
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn1124910777();
--EXPECT--	
int(32767)
Done
