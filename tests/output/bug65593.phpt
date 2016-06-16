--TEST--
Bug #65593 (ob_start(function(){ob_start();});)
--FILE--
<?php

function fn136455393()
{
    echo "Test\n";
    ob_start(function () {
        ob_start();
    });
}
fn136455393();
--EXPECTF--
Test

Fatal error: ob_start(): Cannot use output buffering in output buffering display handlers in %sbug65593.php on line %d

