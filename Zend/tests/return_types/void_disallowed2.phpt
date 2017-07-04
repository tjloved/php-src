--TEST--
void return type: unacceptable cases: explicit return of some other value
--FILE--
<?php

function foo() : void
{
    return -1;
    // not permitted in a void function
}
function fn1506112726()
{
    // Note the lack of function call: function validated at compile-time
}
fn1506112726();
--EXPECTF--
Fatal error: A void function must not return a value in %s on line %d
