--TEST--
Cannot take property of a string
--FILE--
<?php

function fn1275725217()
{
    "foo"->bar;
}
fn1275725217();
--EXPECTF--
Notice: Trying to get property of non-object in %s on line %d
