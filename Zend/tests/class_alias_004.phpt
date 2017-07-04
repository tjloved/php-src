--TEST--
Testing creation of alias using an existing interface name
--FILE--
<?php

class foo
{
}
interface test
{
}
function fn217845313()
{
    class_alias('foo', 'test');
}
fn217845313();
--EXPECTF--
Warning: Cannot declare class test, because the name is already in use in %s on line %d