--TEST--
Catchable fatal error [2]
--FILE--
<?php

class Foo
{
}
function blah(Foo $a)
{
}
function error()
{
    $a = func_get_args();
    var_dump($a);
}
function fn1040256044()
{
    set_error_handler('error');
    try {
        blah(new StdClass());
    } catch (Error $ex) {
        echo $ex->getMessage(), "\n";
    }
    echo "ALIVE!\n";
}
fn1040256044();
--EXPECTF--
Argument 1 passed to blah() must be an instance of Foo, instance of stdClass given, called in %scatchable_error_002.php on line %d
ALIVE!
