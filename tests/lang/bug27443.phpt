--TEST--
Bug #27443 (defined() returns wrong type)
--FILE--
<?php

function fn1905601657()
{
    echo gettype(defined('test'));
}
fn1905601657();
--EXPECT--
boolean
