--TEST--
passing first parameter of __set() by ref
--FILE--
<?php

class test
{
    function __set(&$name, $val)
    {
    }
}
function fn1296253959()
{
    $t = new test();
    $name = "prop";
    $t->{$name} = 1;
    echo "Done\n";
}
fn1296253959();
--EXPECTF--	
Fatal error: Method test::__set() cannot take arguments by reference in %s on line %d
