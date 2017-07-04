--TEST--
Nullable typed return without value generates friendlier error message
--FILE--
<?php

function test() : ?int
{
    return;
}
function fn2125048895()
{
    test();
}
fn2125048895();
--EXPECTF--
Fatal error: A function with return type must return a value (did you mean "return null;" instead of "return;"?) in %s on line %d
