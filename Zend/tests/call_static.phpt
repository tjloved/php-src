--TEST--
__callStatic() Magic method
--FILE--
<?php

class Test
{
    static function __callStatic($fname, $args)
    {
        echo $fname, '() called with ', count($args), " arguments\n";
    }
}
function fn1656710697()
{
    call_user_func("Test::Two", 'A', 'B');
    call_user_func(array("Test", "Three"), NULL, 0, false);
    Test::Four(5, 6, 7, 8);
}
fn1656710697();
--EXPECT--
Two() called with 2 arguments
Three() called with 3 arguments
Four() called with 4 arguments
