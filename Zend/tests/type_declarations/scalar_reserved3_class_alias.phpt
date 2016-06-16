--TEST--
Scalar type names cannot be used as class, trait or interface names (3) - class_alias
--FILE--
<?php

class foobar
{
}
function fn574785549()
{
    class_alias("foobar", "float");
}
fn574785549();
--EXPECTF--
Fatal error: Cannot use 'float' as class name as it is reserved in %s on line %d
