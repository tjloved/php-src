--TEST--
Test wrong number of arguments for ob_get_clean()
--FILE--
<?php

function fn287385121()
{
    /* 
     * proto bool ob_get_clean(void)
     * Function is implemented in main/output.c
    */
    $extra_arg = 1;
    echo "\nToo many arguments\n";
    var_dump(ob_get_clean($extra_arg));
}
fn287385121();
--EXPECTF--

Too many arguments

Warning: ob_get_clean() expects exactly 0 parameters, 1 given in %s on line %d
NULL
