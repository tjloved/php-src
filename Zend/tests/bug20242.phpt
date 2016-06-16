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
function fn54167881()
{
    test::show_static();
    $t = new test();
    $t->show_method();
}
fn54167881();
--EXPECT--
static
method
