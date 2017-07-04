--TEST--
Crash with specific assertions and zend.assertions=-1
--INI--
zend.assertions=-1
--FILE--
<?php

function f()
{
    assert(@$a ?: 1);
    echo "OK";
}
function fn155855109()
{
    f();
}
fn155855109();
--EXPECT--
OK
