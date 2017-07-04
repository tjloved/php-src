--TEST--
Exception during rope finalization
--FILE--
<?php

function fn1408413247()
{
    set_error_handler(function () {
        throw new Exception();
    });
    try {
        $b = "foo";
        $str = "y{$b}{$a}";
    } catch (Exception $e) {
        echo "Exception\n";
    }
}
fn1408413247();
--EXPECT--
Exception
