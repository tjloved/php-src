--TEST--
func_num_args with variable number of args
--FILE--
<?php

function foo($a)
{
    var_dump(func_num_args());
}
function fn583175953()
{
    foo(1, 2, 3);
}
fn583175953();
--EXPECT--
int(3)
