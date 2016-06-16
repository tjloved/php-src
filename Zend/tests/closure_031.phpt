--TEST--
Closure 031: Closure properties with custom error handlers
--FILE--
<?php

function foo($errno, $errstr, $errfile, $errline)
{
    echo "Error: {$errstr}\n";
}
function fn387334472()
{
    set_error_handler('foo');
    $foo = function () {
    };
    try {
        var_dump($foo->a);
    } catch (Error $ex) {
        echo "Error: {$ex->getMessage()}\n";
    }
}
fn387334472();
--EXPECT--
Error: Closure object cannot have properties

