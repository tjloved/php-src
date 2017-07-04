--TEST--
Bug #53727 (Inconsistent behavior of is_subclass_of with interfaces)
--FILE--
<?php

interface MyInterface
{
    const TEST_CONSTANT = true;
}
class ParentClass implements MyInterface
{
}
class ChildClass extends ParentClass
{
}
function fn1327766961()
{
    echo (is_subclass_of('ChildClass', 'MyInterface') ? 'true' : 'false') . "\n";
    echo (defined('ChildClass::TEST_CONSTANT') ? 'true' : 'false') . "\n";
    echo (is_subclass_of('ParentClass', 'MyInterface') ? 'true' : 'false') . "\n";
    echo (defined('ParentClass::TEST_CONSTANT') ? 'true' : 'false') . "\n";
}
fn1327766961();
--EXPECT--
true
true
true
true
