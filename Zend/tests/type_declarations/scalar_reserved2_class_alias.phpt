--TEST--
Scalar type names cannot be used as class, trait or interface names (2) - class_alias
--FILE--
<?php

class foobar
{
}
function fn39182705()
{
    class_alias("foobar", "int");
}
fn39182705();
--EXPECTF--
Fatal error: Cannot use 'int' as class name as it is reserved in %s on line %d
