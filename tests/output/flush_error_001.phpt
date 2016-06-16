--TEST--
Test wrong number of arguments for flush() (no impact)
--FILE--
<?php

function fn1630265448()
{
    /* 
     * proto void flush(void)
     * Function is implemented in ext/standard/basic_functions.c.
     */
    $extra_arg = 1;
    echo "\nToo many arguments\n";
    var_dump(flush($extra_arg));
}
fn1630265448();
--EXPECTF--
Too many arguments

Warning: flush() expects exactly 0 parameters, 1 given in %s on line %d
NULL
