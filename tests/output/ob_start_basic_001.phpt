--TEST--
Test return type and value for ob_start()
--FILE--
<?php

function fn1589499641()
{
    /* 
     * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
     * Function is implemented in main/output.c
    */
    var_dump(ob_start());
}
fn1589499641();
--EXPECT--
bool(true)
