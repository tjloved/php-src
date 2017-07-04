--TEST--
Bug #20242 (Method call in front of class definition)
--FILE--
<?php

class test
{
    static function show_static()
    {
        echo "static\n";
    }
    function show_method()
    {
        echo "method\n";
    }
}
function fn3812258()
{
    test::show_static();
    $t = new test();
    $t->show_method();
}
fn3812258();
--EXPECT--
static
method
