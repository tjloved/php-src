--TEST--
Generators with arguments unpacking
--FILE--
<?php

function fn549257825()
{
    (function () {
        yield;
    })(...range(1, 16384));
    call_user_func_array(function () {
        yield;
    }, range(1, 16384));
    $g = (function () {
        yield;
    })(...range(1, 16384));
    $g = call_user_func_array(function () {
        yield;
    }, range(1, 16384));
    echo "OK\n";
}
fn549257825();
--EXPECT--
OK