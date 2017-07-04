--TEST--
Test return type and value for ob_start()
--FILE--
<?php

function fn501362758()
{
    /* 
     * proto bool ob_start([ string|array user_function [, int chunk_size [, bool erase]]])
     * Function is implemented in main/output.c
    */
    var_dump(ob_start());
}
fn501362758();
--EXPECT--
bool(true)
