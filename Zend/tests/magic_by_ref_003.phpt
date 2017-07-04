--TEST--
passing parameter of __get() by ref
--FILE--
<?php

class test
{
    function __get(&$name)
    {
    }
}
function fn1052329910()
{
    $t = new test();
    $name = "prop";
    var_dump($t->{$name});
    echo "Done\n";
}
fn1052329910();
--EXPECTF--	
Fatal error: Method test::__get() cannot take arguments by reference in %s on line %d
