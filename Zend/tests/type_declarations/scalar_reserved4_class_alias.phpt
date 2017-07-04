--TEST--
Scalar type names cannot be used as class, trait or interface names (4) - class_alias
--FILE--
<?php

class foobar
{
}
function fn1264733174()
{
    class_alias("foobar", "string");
}
fn1264733174();
--EXPECTF--
Fatal error: Cannot use 'string' as class name as it is reserved in %s on line %d
