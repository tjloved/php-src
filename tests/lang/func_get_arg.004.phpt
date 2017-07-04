--TEST--
func_get_arg on non-existent arg
--FILE--
<?php

function foo($a)
{
    var_dump(func_get_arg(2));
}
function fn1589847504()
{
    foo(2, 3);
    echo "\n";
}
fn1589847504();
--EXPECTF--
Warning: func_get_arg():  Argument 2 not passed to function in %s on line %d
bool(false)
