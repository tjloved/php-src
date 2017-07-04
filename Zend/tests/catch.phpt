--TEST--
catch shouldn't call __autoload
--FILE--
<?php

function fn791512274()
{
    spl_autoload_register(function ($name) {
        echo "AUTOLOAD '{$name}'\n";
        eval("class {$name} {}");
    });
    try {
    } catch (A $e) {
    }
    try {
        throw new Exception();
    } catch (B $e) {
    } catch (Exception $e) {
        echo "ok\n";
    }
}
fn791512274();
--EXPECT--
ok
