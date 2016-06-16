--TEST--
passing parameter of __unset() by ref
--FILE--
<?php

class test
{
    function __unset(&$name)
    {
    }
}
function fn2041065826()
{
    $t = new test();
    $name = "prop";
    var_dump($t->{$name});
    echo "Done\n";
}
fn2041065826();
--EXPECTF--	
Fatal error: Method test::__unset() cannot take arguments by reference in %s on line %d
