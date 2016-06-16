--TEST--
Closure 057: segfault in leave helper
--FILE--
<?php

class A
{
}
function getfunc()
{
    $b = function () {
        $a = function () {
        };
        $a();
    };
    return $b->bindTo(new A());
}
function fn545377936()
{
    call_user_func(getfunc());
    echo "okey";
}
fn545377936();
--EXPECT--
okey
