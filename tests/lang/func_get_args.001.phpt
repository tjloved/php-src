--TEST--
func_get_args with no args
--FILE--
<?php

function foo()
{
    var_dump(func_get_args());
}
function fn655021433()
{
    foo();
}
fn655021433();
--EXPECT--
array(0) {
}
