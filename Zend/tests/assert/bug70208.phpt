--TEST--
Bug #70208 (scope information must be preserved with assert())
--FILE--
<?php

function non_class_scope()
{
    return true;
}
class test
{
    protected $prop = 1;
    public function __construct()
    {
        assert('non_class_scope();');
        var_dump($this->prop);
    }
}
function fn1695924508()
{
    new test();
}
fn1695924508();
--EXPECTF--
Deprecated: assert(): Calling assert() with a string argument is deprecated in %s on line %d
int(1)
