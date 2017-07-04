--TEST--
Strlen() function test
--FILE--
<?php

function fn1738432005()
{
    echo strlen("abcdef");
}
fn1738432005();
--EXPECT--
6
