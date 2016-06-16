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
function fn348994792()
{
    error_reporting(E_ALL);
    try {
        @foo(@bar());
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn348994792();
--EXPECT--	
int(32767)
Done
