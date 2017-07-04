--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo
{
}
function fn1869106791()
{
    class_alias('foo', 'FOO');
}
fn1869106791();
--EXPECTF--
Warning: Cannot declare class FOO, because the name is already in use in %s on line %d
