--TEST--
func_num_args with no args
--FILE--
<?php

function foo()
{
    var_dump(func_num_args());
}
function fn850725683()
{
    foo();
}
fn850725683();
--EXPECT--
int(0)
