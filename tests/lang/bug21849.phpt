--TEST--
Bug #21849 (self::constant doesn't work as method's default parameter)
--FILE--
<?php

class foo
{
    const bar = "fubar\n";
    function __construct($arg = self::bar)
    {
        echo $arg;
    }
}
function fn944191077()
{
    new foo();
}
fn944191077();
--EXPECT--
fubar
