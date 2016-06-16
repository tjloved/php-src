--TEST--
passing second parameter of __call() by ref
--FILE--
<?php

class test
{
    function __call($name, &$args)
    {
    }
}
function fn877452303()
{
    $t = new test();
    $func = "foo";
    $arg = 1;
    $t->{$func}($arg);
    echo "Done\n";
}
fn877452303();
--EXPECTF--	
Fatal error: Method test::__call() cannot take arguments by reference in %s on line %d
