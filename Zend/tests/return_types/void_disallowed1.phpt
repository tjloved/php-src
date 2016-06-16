--TEST--
void return type: unacceptable cases: explicit NULL return
--FILE--
<?php

function foo() : void
{
    return NULL;
    // not permitted in a void function

}
function fn282655995()
{
    // Note the lack of function call: function validated at compile-time

}
fn282655995();
--EXPECTF--
Fatal error: A void function must not return a value (did you mean "return;" instead of "return null;"?) in %s on line %d
