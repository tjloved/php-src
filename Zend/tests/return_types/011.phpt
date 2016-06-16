--TEST--
Function returned callable, expected callable

--FILE--
<?php

function foo() : callable
{
    return function () {
    };
}
function fn1865376036()
{
    var_dump(foo());
}
fn1865376036();
--EXPECTF--
object(Closure)#%d (%d) {
}
