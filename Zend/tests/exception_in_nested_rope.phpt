--TEST--
Exception during nested rope
--FILE--
<?php

function fn1899793123()
{
    set_error_handler(function () {
        throw new Exception();
    });
    try {
        $a = "foo";
        $str = "{$a}{${"y{$a}{$a}"}}y";
    } catch (Exception $e) {
        echo "Exception\n";
    }
}
fn1899793123();
--EXPECT--
Exception
