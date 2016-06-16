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
function fn2082566158()
{
    f();
}
fn2082566158();
--EXPECT--
OK
