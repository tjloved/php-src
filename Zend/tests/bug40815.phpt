--TEST--
Bug #40815 (using strings like "class::func" and static methods in set_exception_handler() might result in crash).
--FILE--
<?php

class ehandle
{
    public static function exh($ex)
    {
        echo 'foo';
    }
}
function fn446952502()
{
    set_exception_handler("ehandle::exh");
    throw new Exception("Whiii");
    echo "Done\n";
}
fn446952502();
--EXPECTF--	
foo
