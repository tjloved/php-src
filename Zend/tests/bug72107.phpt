--TEST--
Bug #72107: Segfault when using func_get_args as error handler
--FILE--
<?php

function test($a)
{
    echo $undef;
}
function fn1921150702()
{
    set_error_handler('func_get_args');
    test(1);
}
fn1921150702();
--EXPECTF--
Warning: Cannot call func_get_args() dynamically in %s on line %d

Notice: Undefined variable: undef in %s on line %d
