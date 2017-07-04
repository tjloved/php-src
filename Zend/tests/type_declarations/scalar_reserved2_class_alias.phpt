--TEST--
Scalar type names cannot be used as class, trait or interface names (2) - class_alias
--FILE--
<?php

class foobar
{
}
function fn677671098()
{
    class_alias("foobar", "int");
}
fn677671098();
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
