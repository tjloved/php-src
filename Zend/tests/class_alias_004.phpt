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
function fn536834876()
{
    class_alias('foo', 'test');
}
fn536834876();
--EXPECTF--
Warning: Cannot declare class test, because the name is already in use in %s on line %d