--TEST--
testing @ and error_reporting - 2
--FILE--
<?php

function foo($arg)
{
}
function bar()
{
    error_reporting(E_ALL | E_STRICT);
    throw new Exception("test");
}
function fn1782317381()
{
    error_reporting(E_ALL);
    try {
        @foo(@bar());
    } catch (Exception $e) {
    }
    var_dump(error_reporting());
    echo "Done\n";
}
fn1782317381();
--EXPECT--	
int(32767)
Done
