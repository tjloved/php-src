--TEST--
Bug #52940 (call_user_func_array still allows call-time pass-by-reference)
--FILE--
<?php

function foo($a)
{
    $a++;
    var_dump($a);
}
function bar(&$a)
{
    $a++;
    var_dump($a);
}
function fn1058812154()
{
    $a = 1;
    call_user_func_array("foo", array(&$a));
    var_dump($a);
    call_user_func_array("bar", array(&$a));
    var_dump($a);
}
fn1058812154();
--EXPECT--
int(2)
int(1)
int(2)
int(2)
