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
function fn881148957()
{
    call_user_func(getfunc());
    echo "okey";
}
fn881148957();
--EXPECT--
okey
