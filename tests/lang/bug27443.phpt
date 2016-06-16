--TEST--
Bug #27443 (defined() returns wrong type)
--FILE--
<?php

function fn65524687()
{
    echo gettype(defined('test'));
}
fn65524687();
--EXPECT--
boolean
