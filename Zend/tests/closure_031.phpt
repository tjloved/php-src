--TEST--
Closure 031: Closure properties with custom error handlers
--FILE--
<?php

function foo($errno, $errstr, $errfile, $errline)
{
    echo "Error: {$errstr}\n";
}
function fn270506506()
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
fn270506506();
--EXPECT--
Error: Closure object cannot have properties

