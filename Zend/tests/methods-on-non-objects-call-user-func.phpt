--TEST--
call_user_func() in combination with "Call to a member function method() on a non-object"
--FILE--
<?php

function fn1570151553()
{
    $comparator = null;
    var_dump(call_user_func([$comparator, 'compare'], 1, 2));
    echo "Alive\n";
}
fn1570151553();
--EXPECTF--
Warning: call_user_func() expects parameter 1 to be a valid callback, first array member is not a valid class name or object in %s on line %d
NULL
Alive

