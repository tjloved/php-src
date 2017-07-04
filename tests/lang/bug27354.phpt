--TEST--
Bug #27354 (Modulus operator crashes PHP)
--FILE--
<?php

function fn2066601787()
{
    var_dump(-2147483647 % -1);
    var_dump(-2147483649 % -1);
    var_dump(-2147483648 % -1);
    var_dump(-2147483648 % -2);
}
fn2066601787();
--EXPECTF--
int(%i)
int(%i)
int(%i)
int(%i)
