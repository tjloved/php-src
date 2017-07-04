--TEST--
Function returned callable, expected callable

--FILE--
<?php

function foo() : callable
{
    return function () {
    };
}
function fn1261970123()
{
    var_dump(foo());
}
fn1261970123();
--EXPECTF--
object(Closure)#%d (%d) {
}
