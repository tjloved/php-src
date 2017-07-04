--TEST--
Bug #26866 (segfault when exception raised in __get)
--FILE--
<?php

class bar
{
    function get_name()
    {
        return 'bar';
    }
}
class foo
{
    function __get($sName)
    {
        throw new Exception('Exception!');
        return new bar();
    }
}
function fn1059366108()
{
    $foo = new foo();
    try {
        echo $foo->bar->get_name();
    } catch (Exception $E) {
        echo "Exception raised!\n";
    }
}
fn1059366108();
--EXPECT--
Exception raised!
