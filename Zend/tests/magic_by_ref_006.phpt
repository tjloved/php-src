--TEST--
passing first parameter of __call() by ref
--FILE--
<?php

class test
{
    function __call(&$name, $args)
    {
    }
}
function fn1640840629()
{
    $t = new test();
    $func = "foo";
    $t->{$func}();
    echo "Done\n";
}
fn1640840629();
--EXPECTF--	
Fatal error: Method test::__call() cannot take arguments by reference in %s on line %d
