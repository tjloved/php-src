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
function fn1322961497()
{
    ['Test', 'method']();
    ('Test::method')();
    ['Test', 'method']();
    ('Test::method')();
}
fn1322961497();
--EXPECT--
Method called!
Method called!
Method called!
Method called!
