--TEST--
Error message for isset(func())
--FILE--
<?php

function fn7019900()
{
    isset(abc());
}
fn7019900();
--EXPECTF--
Fatal error: Cannot use isset() on the result of an expression (you can use "null !== expression" instead) in %s on line %d
