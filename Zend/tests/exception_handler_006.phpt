--TEST--
exception handler tests - 6
--FILE--
<?php

function foo($e)
{
    var_dump(__FUNCTION__ . "(): " . get_class($e) . " thrown!");
}
function foo1($e)
{
    var_dump(__FUNCTION__ . "(): " . get_class($e) . " thrown!");
}
function fn2066407237()
{
    set_exception_handler("foo");
    set_exception_handler("foo1");
    restore_exception_handler();
    throw new excEption();
    echo "Done\n";
}
fn2066407237();
--EXPECTF--	
string(24) "foo(): Exception thrown!"
