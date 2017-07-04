--TEST--
By-ref variadics enforce the reference
--FILE--
<?php

function test(&...$args)
{
}
function fn1275942055()
{
    test(1);
}
fn1275942055();
--EXPECTF--
Fatal error: Only variables can be passed by reference in %s on line %d
