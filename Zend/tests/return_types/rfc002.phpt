--TEST--
RFC example: Scalar Types

--FILE--
<?php

function answer() : int
{
    return 42;
}
function fn1827321962()
{
    var_dump(answer());
}
fn1827321962();
--EXPECTF--
int(42)
