--TEST--
Exception during rope finalization
--FILE--
<?php

function fn654936852()
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
fn654936852();
--EXPECT--
Exception
