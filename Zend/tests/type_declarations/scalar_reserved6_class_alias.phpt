--TEST--
Scalar type names cannot be used as class, trait or interface names (6) - class_alias
--FILE--
<?php

class foobar
{
}
function fn1320581999()
{
    class_alias("foobar", "bool");
}
fn1320581999();
--EXPECTF--
Fatal error: Cannot use 'bool' as class name as it is reserved in %s on line %d
