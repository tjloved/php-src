--TEST--
Cannot take property of a string
--FILE--
<?php

function fn166003633()
{
    "foo"->bar;
}
fn166003633();
--EXPECTF--
Notice: Trying to get property of non-object in %s on line %d
