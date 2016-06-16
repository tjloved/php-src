--TEST--
Trying redeclare class with class_alias()
--FILE--
<?php

class foo
{
}
function fn450070711()
{
    class_alias('foo', 'FOO');
}
fn450070711();
--EXPECTF--
Warning: Cannot declare class FOO, because the name is already in use in %s on line %d
