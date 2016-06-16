--TEST--
By-ref variadics enforce the reference
--FILE--
<?php

function test(&...$args)
{
}
function fn802256047()
{
    test(1);
}
fn802256047();
--EXPECTF--
Fatal error: Only variables can be passed by reference in %s on line %d
