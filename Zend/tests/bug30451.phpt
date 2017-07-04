--TEST--
Bug #30451 (static properties permissions broken)
--FILE--
<?php

class A
{
    protected static $property = TRUE;
    protected static function method()
    {
        return TRUE;
    }
}
class B extends A
{
    public function __construct()
    {
        var_dump(self::method());
        var_dump(parent::method());
        var_dump(self::$property);
        var_dump(parent::$property);
    }
}
function fn1231220991()
{
    new B();
}
fn1231220991();
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
