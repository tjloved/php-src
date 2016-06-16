--TEST--
Closure 029: Testing lambda with instanceof operator
--FILE--
<?php

function fn300536179()
{
    var_dump(function () {
    } instanceof closure);
    var_dump(function (&$x) {
    } instanceof closure);
    var_dump(@function (&$x) use($y, $z) {
    } instanceof closure);
}
fn300536179();
--EXPECT--
bool(true)
bool(true)
bool(true)
