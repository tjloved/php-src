--TEST--
Strlen() function test
--FILE--
<?php

function fn676373775()
{
    echo strlen("abcdef");
}
fn676373775();
--EXPECT--
6
