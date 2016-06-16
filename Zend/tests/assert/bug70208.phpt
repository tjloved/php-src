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
function fn972693701()
{
    new test();
}
fn972693701();
--EXPECT--
int(1)
