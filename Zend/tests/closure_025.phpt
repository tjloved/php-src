--TEST--
Closure 025: Using closure in create_function()
--FILE--
<?php

function fn2031133777()
{
    $a = create_function('$x', 'return function($y) use ($x) { return $x * $y; };');
    var_dump($a(2)->__invoke(4));
}
fn2031133777();
--EXPECTF--
Deprecated: Function create_function() is deprecated in %s on line %d
int(8)
