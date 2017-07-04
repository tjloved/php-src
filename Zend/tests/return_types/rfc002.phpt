--TEST--
RFC example: Scalar Types

--FILE--
<?php

function answer() : int
{
    return 42;
}
function fn1725753520()
{
    var_dump(answer());
}
fn1725753520();
--EXPECTF--
int(42)
