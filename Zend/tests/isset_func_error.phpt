--TEST--
Error message for isset(func())
--FILE--
<?php

function fn1037822824()
{
    isset(abc());
}
fn1037822824();
--EXPECTF--
Fatal error: Cannot use isset() on the result of an expression (you can use "null !== expression" instead) in %s on line %d
