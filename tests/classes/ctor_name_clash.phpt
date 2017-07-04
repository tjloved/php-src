--TEST--
ZE2 The child class can re-use the parent class name for a function member
--FILE--
<?php

class base
{
    function base()
    {
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
    }
}
class derived extends base
{
    function base()
    {
        echo __CLASS__ . "::" . __FUNCTION__ . "\n";
    }
}
function fn1945197340()
{
    $obj = new derived();
    $obj->base();
}
fn1945197340();
--EXPECTF--
Deprecated: Methods with the same name as their class will not be constructors in a future version of PHP; base has a deprecated constructor in %s on line %d
base::base
derived::base
