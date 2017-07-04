--TEST--
Testing nested functions
--FILE--
<?php

function F()
{
    $a = "Hello ";
    return $a;
}
function G()
{
    static $myvar = 4;
    echo "{$myvar} ";
    echo F();
    echo "{$myvar}";
}
function fn442680580()
{
    G();
}
fn442680580();
--EXPECT--
4 Hello 4
