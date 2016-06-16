--TEST--
passing parameter of __isset() by ref
--FILE--
<?php

class test
{
    function __isset(&$name)
    {
    }
}
function fn1877735797()
{
    $t = new test();
    $name = "prop";
    var_dump(isset($t->{$name}));
    echo "Done\n";
}
fn1877735797();
--EXPECTF--	
Fatal error: Method test::__isset() cannot take arguments by reference in %s on line %d
