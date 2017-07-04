--TEST--
func_num_args with variable number of args
--FILE--
<?php

function foo($a)
{
    var_dump(func_num_args());
}
function fn1328525966()
{
    foo(1, 2, 3);
}
fn1328525966();
--EXPECT--
int(3)
