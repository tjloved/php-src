--TEST--
func_num_args with no args
--FILE--
<?php

function foo()
{
    var_dump(func_num_args());
}
function fn242602511()
{
    foo();
}
fn242602511();
--EXPECT--
int(0)
