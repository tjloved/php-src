--TEST--
Bug #65593 (ob_start(function(){ob_start();});)
--FILE--
<?php

function fn1886605164()
{
    echo "Test\n";
    ob_start(function () {
        ob_start();
    });
}
fn1886605164();
--EXPECTF--
Test

Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %sbug65593.php on line %d

