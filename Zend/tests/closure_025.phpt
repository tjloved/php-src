--TEST--
Closure 025: Using closure in create_function()
--FILE--
<?php

function fn434321250()
{
    $a = create_function('$x', 'return function($y) use ($x) { return $x * $y; };');
    var_dump($a(2)->__invoke(4));
}
fn434321250();
--EXPECT--
int(8)
