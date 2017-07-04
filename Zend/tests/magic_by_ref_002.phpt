--TEST--
passing second parameter of __set() by ref
--FILE--
<?php

class test
{
    function __set($name, &$val)
    {
    }
}
function fn167595673()
{
    $t = new test();
    $t->prop = 1;
    echo "Done\n";
}
fn167595673();
--EXPECTF--	
Fatal error: Method test::__set() cannot take arguments by reference in %s on line %d
