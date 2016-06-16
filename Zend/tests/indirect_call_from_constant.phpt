--TEST--
Indirect call with constants.
--FILE--
<?php

class Test
{
    public static function method()
    {
        echo "Method called!\n";
    }
}
function fn544461077()
{
    ['Test', 'method']();
    ('Test::method')();
    ['Test', 'method']();
    ('Test::method')();
}
fn544461077();
--EXPECT--
Method called!
Method called!
Method called!
Method called!
