--TEST--
Closure 029: Testing lambda with instanceof operator
--FILE--
<?php

function fn542637576()
{
    var_dump(function () {
    } instanceof closure);
    var_dump(function (&$x) {
    } instanceof closure);
    var_dump(@function (&$x) use($y, $z) {
    } instanceof closure);
}
fn542637576();
--EXPECT--
bool(true)
bool(true)
bool(true)
